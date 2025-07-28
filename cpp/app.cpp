#include <csignal>
#include <fstream>
#include "audio_feeder.hpp"
#include "conductor.hpp"
#include "signaling_client.hpp"
#include "peer_connection.hpp"
#include "rtc_base/ssl_adapter.h"

void writeWavFile(const std::vector<Sample>& samples, const std::string& filename, int samplerate, int chan, int bits) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
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

int main(int argc, const char** argv) {
    rtc::InitializeSSL();

    struct lws_context_creation_info info;
    memset(&info, 0, sizeof info);
    lws_cmdline_option_handle_builtin(argc, argv, &info);

    Conductor *conductor = new Conductor();
    std::shared_ptr<SignalingClientInterface> signaling_client = std::make_shared<SignalingClient>(conductor);
    rtc::scoped_refptr<PeerConnection> peer_connection = rtc::make_ref_counted<PeerConnection>(conductor);
    peer_connection->AddRef();
    conductor->set_peer_connection(peer_connection.get());
    conductor->set_signaling_client(signaling_client.get());

    peer_connection->call();
    signaling_client->connect("192.168.0.45", "signaling", 8444);

    AudioConfig config{48000, 1, 480, 16}; // 48 kHz, mono, 480 frames, 16-bit PCM
    AudioFeeder feeder{config};

    for (auto dev : feeder.get_audio_devices()) {
        std::cout << dev.str() << std::endl;
    }

    feeder.set_sink(peer_connection.get());

    for (AudioDeviceInfo device : feeder.get_audio_devices()) {
        if (std::string(device.getName()).find("sof-hda-dsp Digital Microphone") != std::string::npos) {
            feeder.start(device);
            break;
        }
    }

    Pa_Sleep(10000); // Record for 10 seconds

    writeWavFile(*feeder.data_->data, "output.wav", feeder.config_.getSampleRate(), feeder.config_.getChannels(), feeder.config_.getFormat());

    rtc::CleanupSSL();
    return 0;
}