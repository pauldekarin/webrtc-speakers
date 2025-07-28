//
// Created by bimba on 7/26/25.
//

#include "audio_feeder.hpp"

#include <iostream>


bool AudioFeeder::library_loaded = false;

AudioFeeder::AudioFeeder(AudioConfig config):AudioFeeder(config, nullptr)
{
}


AudioFeeder::AudioFeeder(AudioConfig config, AudioHandlerInterface* sink):
    sink_(sink), config_(config), stream_(nullptr), data_(new paData{.feeder = this})
{
    if (!AudioFeeder::library_loaded)
    {
        AudioFeeder::library_loaded = AudioFeeder::load_library_();
    }
}

void AudioFeeder::start(AudioDeviceInfo info)
{

    PaStreamParameters  inputParameters,
                        outputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    int                 i;
    int                 totalFrames;
    int                 numSamples;
    int                 numBytes;
    SAMPLE              max, val;
    double              average;
    this->data_->data = new std::vector<Sample>();

    printf("patest_record.c\n"); fflush(stdout);

    inputParameters.device = paNoDevice;

    for (int i = 0; i < Pa_GetDeviceCount(); i++)
    {
        const PaDeviceInfo *p_info = Pa_GetDeviceInfo(i);

        if (std::string{p_info->name}.find("Monitor of sof-hda-dsp Speaker + Headphones") != std::string::npos)
        {
            std::cout << "FOUND!" << std::endl;
            std::cout << p_info->maxInputChannels << std::endl;
            inputParameters.device = i;
            break;
        }
    }
    // inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
    }

    PaSampleFormat format = paInt16;

    if (this->config_.getFormat() == 32)
    {
        format = paFloat32;
    }else if (this->config_.getFormat() == 24)
    {
        format = paInt24;
    }

    inputParameters.channelCount = this->config_.getChannels();
    inputParameters.sampleFormat = format;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;


    /* Record some audio. -------------------------------------------- */
    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,                  /* &outputParameters, */
              this->config_.getSampleRate(),
              this->config_.getFramesPerBuffer(),
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              AudioFeeder::callback_,
              this->data_ );
    if( err != paNoError )
    {
        std::cerr << "Failed Pa_OpenStream()\n" << err << std::endl;
        return;
    }

    err = Pa_StartStream( stream );
    if( err != paNoError ) return;;
    printf("\n=== Now recording!! Please speak into the microphone. ===\n"); fflush(stdout);

    while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
    {
        Pa_Sleep(1000);
        printf("index \n" ); fflush(stdout);
    }

    Pa_CloseStream( stream );
}
//     if (!this->is_available())
//     {
//         throw std::runtime_error("AudioFeeder::start: Device is not available");
//     }
//
//     std::cout << __func__ << std::endl;
//
//     const PaDeviceInfo *p_info = AudioFeeder::map_info_(info);
//
//     if (!p_info)
//     {
//         std::cerr << "AudioFeeder::start: Failed to map device info" << std::endl;
//         return;
//     }
//
//     PaError err;
//     PaStream *stream;
//     PaStreamParameters parameters;
//
//     unsigned long sampleFormat;
//
//     if (this->config_.getFormat() == 32)
//     {
//         sampleFormat = paFloat32;
//     }else if (this->config_.getFormat() == 24)
//     {
//         sampleFormat = paInt24;
//     }else
//     {
//         sampleFormat = paInt16;
//     }
//
//     parameters.channelCount = this->config_.getChannels();
//     parameters.sampleFormat = sampleFormat;
//     parameters.device = info.getIndex();
//     parameters.suggestedLatency = p_info->defaultLowInputLatency;
//     parameters.hostApiSpecificStreamInfo = nullptr;
//     this->data_->data  = new std::vector<Sample>();
//
//     std::cout << "Pa_OpenStream() " << p_info->name << std::endl;
//     err = Pa_OpenStream(&stream, &parameters,nullptr, this->config_.getSampleRate(), this->config_.getFramesPerBuffer(), paClipOff, AudioFeeder::callback_, this->data_);
//
//     if (err != paNoError)
//     {
//         std::cerr << "Pa_OpenStream failed " << std::endl;
//         return;
//     }
//
//     std::cout << "Pa_StartStream()..." << std::endl;
//     err = Pa_StartStream(stream);
//
//     if (err != paNoError)
//     {
//         std::cerr << "Pa_StartStream failed " << std::endl;
//         return;
//     }
//
//     this->stream_ = stream;
//
//     std::cout << "Started name: " << p_info->name << " index: " << info.getIndex() << std::endl;
// }

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

    this->sink_->handle(buffer, this->config_.getFormat(), this->config_.getSampleRate(), this->config_.getChannels(), frames_per_buffer);
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

    for (int i =0 ; i < num_devices; i++)
    {
        const PaDeviceInfo *p_info = Pa_GetDeviceInfo(i);
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
                           const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags, void* user_data) {
    if (!user_data) {
        std::cerr << "user_data is undefined" << std::endl;
        return paComplete;
    }


    AudioFeeder *feeder = static_cast<paData*>(user_data)->feeder;
    if (!feeder) {
        std::cerr << "user_data does not contain feeder" << std::endl;
        return paComplete;
    }

    static int i = 0;

    i += frames_per_buffer;

    if (i > feeder->config_.getSampleRate() * 20)
    {
        return paComplete;
    }


    std::vector<Sample>* recordedSamples = static_cast<std::vector<Sample>*>(static_cast<paData*>(user_data)->data);
    const Sample *input = static_cast<const Sample*>(input_buffer);

    if (!input) {
        std::cerr << "No input data available" << std::endl;
        feeder->on_data(nullptr, frames_per_buffer);
        return paContinue;
    }

    // Normalize samples to [-1.0, 1.0] if necessary
    for (unsigned long i = 0; i < frames_per_buffer * feeder->config_.getChannels(); i++) {
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
        const PaDeviceInfo *p_info = Pa_GetDeviceInfo(i);

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
