//
// Created by bimba on 7/26/25.
//

#include "audio_feeder.hpp"

#include <iostream>

#include "spdlog/spdlog.h"


bool AudioFeeder::library_loaded = false;

AudioFeeder::AudioFeeder(AudioConfig config): AudioFeeder(config, nullptr)
{
}


AudioFeeder::AudioFeeder(AudioConfig config, AudioHandlerInterface* sink):
    sink_(sink), config_(config), stream_(nullptr), data_(new paData{.feeder = this})
{
    this->setup_logger_();

    if (!library_loaded)
    {
        library_loaded = AudioFeeder::load_library_();

        if (!library_loaded)
        {
            this->logger_->info("Loaded library");

            throw new std::runtime_error("AudioFeeder::load_library_() failed");
        }

        this->logger_->info("Loaded library");
    }
}

void AudioFeeder::start(AudioDeviceInfo info)
{
    if (!this->is_available())
    {
        this->logger_->error("Environment is not ready");
        throw std::runtime_error("AudioFeeder::start Device is not available");
    }

    std::cout << __func__ << std::endl;

    const PaDeviceInfo* p_info = AudioFeeder::map_info_(info);

    if (!p_info)
    {
        this->logger_->error("Failed to setup required device info");

        throw std::runtime_error("AudioFeeder::map_info_() failed");
    }

    PaError err;
    PaStream* stream;
    PaStreamParameters parameters;
    PaSampleFormat sampleFormat;

    if (this->config_.getFormat() == 32)
    {
        sampleFormat = paFloat32;
    }
    else if (this->config_.getFormat() == 24)
    {
        sampleFormat = paInt24;
    }
    else
    {
        sampleFormat = paInt16;
    }

    parameters.channelCount = this->config_.getChannels();
    parameters.sampleFormat = sampleFormat;
    parameters.device = info.getIndex();
    parameters.suggestedLatency = p_info->defaultLowInputLatency;
    parameters.hostApiSpecificStreamInfo = nullptr;

    this->data_->data = new std::vector<Sample>();

    this->logger_->info("Trying to open stream");
    err = Pa_OpenStream(&stream, &parameters, nullptr, this->config_.getSampleRate(),
                        this->config_.getFramesPerBuffer(), paClipOff, AudioFeeder::callback_, this->data_);

    if (err != paNoError)
    {
        this->logger_->error("Failed to open stream. Error: {}", Pa_GetErrorText(err));

        throw std::runtime_error("AudioFeeder::open_stream() failed");
    }

    this->logger_->info("Trying to open stream");

    err = Pa_StartStream(stream);

    if (err != paNoError)
    {
        this->logger_->error("Failed to start stream. Error: {}", Pa_GetErrorText(err));

        throw std::runtime_error("AudioFeeder::start_stream() failed");
    }

    this->stream_ = stream;

    this->logger_->debug("Started streaming on {}", info.str());
}

void AudioFeeder::stop()
{
    if (!this->stream_)
    {
        return;
    }

    PaError error = Pa_StopStream(this->stream_);

    if (error != paNoError)
    {
        std::cerr << "Pa_StopStream failed " << std::endl;
        return;
    }

    this->stream_ = nullptr;
}

bool AudioFeeder::is_available()
{
    return AudioFeeder::library_loaded;
}

bool AudioFeeder::is_feeding()
{
    return this->stream_ != nullptr && Pa_IsStreamActive(this->stream_);
}

void AudioFeeder::set_sink(AudioHandlerInterface* sink)
{
    this->sink_ = sink;
}

void AudioFeeder::on_data(const void* buffer, unsigned long frames_per_buffer)
{
    if (!this->sink_ || !buffer)
    {
        std::cout << this->sink_ << " " << buffer << std::endl;
        return;
    }

    this->sink_->handle(buffer, this->config_.getFormat(), this->config_.getSampleRate(), this->config_.getChannels(),
                        frames_per_buffer);
}

const AudioConfig AudioFeeder::get_audio_config() const
{
    return this->config_;
}

std::vector<AudioDeviceInfo> AudioFeeder::get_audio_devices()
{
    if (!this->is_available())
    {
        throw std::runtime_error("Failed to getAudioDevices since library is not loaded");
    }

    std::vector<AudioDeviceInfo> devices;
    int num_devices = Pa_GetDeviceCount();

    for (int i = 0; i < num_devices; i++)
    {
        const PaDeviceInfo* p_info = Pa_GetDeviceInfo(i);
        std::string name(p_info->name);
        AudioDeviceInfo info(name, i);
        devices.push_back(info);
    }

    return devices;
}

AudioDeviceInfo AudioFeeder::get_default_output_device()
{
    if (!this->is_available())
    {
        throw std::runtime_error("Failed to getDefaultOutputDevice since library is not loaded");
    }

    return map_info_(Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice()));
}

AudioDeviceInfo AudioFeeder::get_default_input_device()
{
    return map_info_(Pa_GetDeviceInfo(Pa_GetDefaultInputDevice()));
}

int AudioFeeder::callback_(const void* input_buffer, void* output_buffer, unsigned long frames_per_buffer,
                           const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
                           void* user_data)
{
    if (!user_data)
    {
        std::cerr << "user_data is undefined" << std::endl;
        return paComplete;
    }


    AudioFeeder* feeder = static_cast<paData*>(user_data)->feeder;
    if (!feeder)
    {
        std::cerr << "user_data does not contain feeder" << std::endl;
        return paComplete;
    }

    static int i = 0;

    i += frames_per_buffer;

    if (i > feeder->config_.getSampleRate() * 2000)
    {
        return paComplete;
    }


    std::vector<Sample>* recordedSamples = static_cast<std::vector<Sample>*>(static_cast<paData*>(user_data)->data);
    const Sample* input = static_cast<const Sample*>(input_buffer);

    if (!input)
    {
        std::cerr << "No input data available" << std::endl;
        feeder->on_data(nullptr, frames_per_buffer);
        return paContinue;
    }

    // Normalize samples to [-1.0, 1.0] if necessary
    for (unsigned long i = 0; i < frames_per_buffer * feeder->config_.getChannels(); i++)
    {
        recordedSamples->push_back(input[i]);
    }

    feeder->on_data(input_buffer, frames_per_buffer);
    return paContinue;
}

int AudioFeeder::record_(const void* input_buffer, void* output_buffer, unsigned long frames_per_buffer,
                         const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags, void* user_data)
{
}

bool AudioFeeder::load_library_()
{
    PaError err;
    err = Pa_Initialize();

    if (err != paNoError)
    {
        std::cerr << "Failed to Pa_Initialize()" << std::endl;
        return false;
    }

    return true;
}

AudioDeviceInfo AudioFeeder::map_info_(const PaDeviceInfo* info)
{
    int num_devices = Pa_GetDeviceCount();
    int index = 0;

    for (int i = 0; i < num_devices; i++)
    {
        const PaDeviceInfo* p_info = Pa_GetDeviceInfo(i);

        if (p_info->name == info->name)
        {
            index = i;
            break;
        }
    }
    return {info->name, index};
}

const PaDeviceInfo* AudioFeeder::map_info_(AudioDeviceInfo info)
{
    int num_devices = Pa_GetDeviceCount();

    for (int i = 0; i < num_devices; i++)
    {
        const PaDeviceInfo* p_info = Pa_GetDeviceInfo(i);

        if (p_info->name == info.getName())
        {
            return p_info;
        }
    }

    return nullptr;
}

void AudioFeeder::setup_logger_()
{
    this->logger_ = spdlog::create<log_sink>(this->logger_name_);
}
