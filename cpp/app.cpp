#include <csignal>
#include <fstream>
#include "audio_feeder.hpp"
#include "conductor.hpp"
#include "signaling_client.hpp"
#include "rtc_client.hpp"
#include "settings.hpp"
#include "virtual_audio_output.hpp"
#include "rtc_base/ssl_adapter.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"

void writeWavFile(const std::vector<Sample>& samples, const std::string& filename, int samplerate, int chan, int bits)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open WAV file for writing" << std::endl;
        return;
    }

    // WAV header
    uint32_t sampleRate = samplerate;
    uint16_t channels = chan; // Mono
    uint16_t bitsPerSample = bits; // 16-bit samples
    uint32_t dataSize = samples.size() * sizeof(Sample);
    uint32_t fileSize = dataSize + 44;

    file.write("RIFF", 4);
    file.write(reinterpret_cast<const char*>(&fileSize), 4);
    file.write("WAVEfmt ", 8);
    uint32_t fmtSize = 16;
    file.write(reinterpret_cast<const char*>(&fmtSize), 4);
    uint16_t audioFormat = 1; // PCM (not IEEE float)
    file.write(reinterpret_cast<const char*>(&audioFormat), 2);
    file.write(reinterpret_cast<const char*>(&channels), 2);
    file.write(reinterpret_cast<const char*>(&sampleRate), 4);
    uint32_t byteRate = sampleRate * channels * bitsPerSample / 8;
    file.write(reinterpret_cast<const char*>(&byteRate), 4);
    uint16_t blockAlign = channels * bitsPerSample / 8;
    file.write(reinterpret_cast<const char*>(&blockAlign), 2);
    file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);
    file.write("data", 4);
    file.write(reinterpret_cast<const char*>(&dataSize), 4);

    // Write samples
    file.write(reinterpret_cast<const char*>(samples.data()), dataSize);
    file.close();
    std::cout << "Recording saved to " << filename << std::endl;
}

#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"

static int playCallback(const void* inputBuffer, void* outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void* userData)
{
    AudioFeeder* feeder = static_cast<AudioFeeder*>(static_cast<paData*>(userData)->feeder);
    std::vector<SAMPLE>* recordedSamples = static_cast<std::vector<SAMPLE>*>(static_cast<paData*>(userData)->data);
    static size_t frameIndex = 0; // Track current position in the vector
    SAMPLE* wptr = static_cast<SAMPLE*>(outputBuffer);
    size_t framesLeft = recordedSamples->size() / feeder->config_.getChannels() - frameIndex;
    unsigned int i;
    int finished;

    (void)inputBuffer; // Prevent unused variable warnings
    (void)timeInfo;
    (void)statusFlags;

    if (framesLeft < framesPerBuffer)
    {
        // Final buffer: copy remaining samples and pad with zeros
        for (i = 0; i < framesLeft; i++)
        {
            *wptr++ = (*recordedSamples)[frameIndex * feeder->config_.getChannels() + i];
            if (feeder->config_.getChannels() == 2)
                *wptr++ = (*recordedSamples)[frameIndex * feeder->config_.
                                                                  getChannels() + i + 1];
        }
        for (; i < framesPerBuffer; i++)
        {
            *wptr++ = 0; // Left channel
            if (feeder->config_.getChannels() == 2) *wptr++ = 0; // Right channel
        }
        frameIndex += framesLeft;
        finished = paComplete;
    }
    else
    {
        // Copy samples for the current buffer
        for (i = 0; i < framesPerBuffer; i++)
        {
            *wptr++ = (*recordedSamples)[frameIndex * feeder->config_.getChannels() + i];
            if (feeder->config_.getChannels() == 2)
                *wptr++ = (*recordedSamples)[frameIndex * feeder->config_.
                                                                  getChannels() + i + 1];
        }
        frameIndex += framesPerBuffer;
        finished = paContinue;
    }

    // Reset frameIndex when playback is complete
    if (finished == paComplete)
    {
        frameIndex = 0;
    }

    return finished;
}

class App
{
public:
    App();
    ~App();

    void connect_to_signaling_server();
    void register_native_connection();
    void select_audio_output_device();
    void create_virtual_audio_output();
    void run();


    void setup_logger_();
    void setup_environment_();
    void terminate_();

    std::unique_ptr<Conductor> conductor_;
    std::unique_ptr<SignalingClientInterface> signaling_client_;
    std::shared_ptr<AudioFeeder> audio_feeder_;
    rtc::scoped_refptr<RtcClient> peer_connection_;
    std::shared_ptr<VirtualAudioOutput> virtual_audio_output_;

    std::shared_ptr<spdlog::logger> logger_;
    const std::string logger_name_ = "App";
};

App::App():
    peer_connection_(nullptr), signaling_client_(nullptr), conductor_(nullptr), virtual_audio_output_(nullptr)
{
    this->setup_logger_();
    this->setup_environment_();
}

App::~App()
{
    this->terminate_();
}

void App::connect_to_signaling_server()
{
    if (!this->signaling_client_)
    {
        this->logger_->error("Signaling client is nullptr");

        throw std::runtime_error("Signaling client is nullptr");
    }

    this->logger_->info("Setting up connection with signaling server");

    this->signaling_client_->connect(Settings::get_signaling_server_url());
}

void App::register_native_connection()
{
    if (!this->signaling_client_)
    {
        this->logger_->error("Signaling client is undefined");
        throw new std::runtime_error("Signaling client is undefined");
    }

    nlohmann::json data = {
        {"type", "register_native"}
    };

    this->signaling_client_->send(data.dump());
}

void App::select_audio_output_device()
{
    std::vector<AudioDeviceInfo> devices = this->audio_feeder_->get_audio_devices();

    if (devices.empty())
    {
        this->logger_->error("No audio devices found");

        throw new std::runtime_error("No audio devices found");
    }

    this->logger_->info("Type audio device info index");

    for (int i = 0; i < devices.size(); i++)
    {
        this->logger_->info(" #{} {}", i, devices[i].str());
    }

    int index;
    std::cin >> index;

    if (index < 0 || index >= devices.size())
    {
        this->logger_->error("Invalid index");

        throw new std::invalid_argument("Invalid index");
    }

    this->audio_feeder_->start(devices[index]);
}

void App::create_virtual_audio_output()
{
    this->virtual_audio_output_ = std::make_shared<VirtualAudioOutput>();
}

void App::run()
{
    while (std::cin.get() != 'q')
    {
    }
}

void App::setup_logger_()
{
    this->logger_ = spdlog::create<spdlog::sinks::stdout_color_sink_mt>(this->logger_name_);
}

void App::setup_environment_()
{
    this->logger_->info("Setting up environment");

    rtc::InitializeSSL();

    this->conductor_ = std::make_unique<Conductor>();

    this->signaling_client_ = std::make_unique<SignalingClient>(this->conductor_.get());
    this->audio_feeder_ = std::make_shared<AudioFeeder>(AudioConfig{48000, 2, 480, 16});

    this->conductor_->set_signaling_client(this->signaling_client_.get());
    this->conductor_->set_audio_feeder(this->audio_feeder_);
}

void App::terminate_()
{
    this->logger_->info("Terminating");

    if (this->peer_connection_)
    {
        webrtc::RefCountReleaseStatus status = this->peer_connection_->Release();
        if (status != webrtc::RefCountReleaseStatus::kDroppedLastRef)
        {
            spdlog::error("PeerConnection has remained references, but this processing is going to be terminated");
        }
    }

    if (this->signaling_client_)
    {
        this->signaling_client_->disconnect();
    }

    rtc::CleanupSSL();
}


int main(int argc, const char** argv)
{
    Settings::load(argc, argv);

    App app{};
    // app.create_virtual_audio_output();
    app.select_audio_output_device();
    app.connect_to_signaling_server();
    app.register_native_connection();
    app.run();

    std::cin.get();


    return 0;
}







