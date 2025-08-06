//
// Created by bimba on 27.06.25.
//

#include "audio_source.hpp"


void AudioSource::send(const void* audio_data, int bits_per_sample, int sample_rate, size_t number_of_channels,
                       size_t number_of_frames)
{
    if (!this->sink_ || !audio_data)
    {
        return;
    }

    this->sink_->OnData(audio_data, bits_per_sample, sample_rate, number_of_channels, number_of_frames);
}

void AudioSource::RegisterObserver(webrtc::ObserverInterface* observer)
{
}

void AudioSource::UnregisterObserver(webrtc::ObserverInterface* observer)
{
}

webrtc::MediaSourceInterface::SourceState AudioSource::state() const
{
    return webrtc::MediaSourceInterface::SourceState::kLive;
}

bool AudioSource::remote() const
{
    return false;
}

void AudioSource::SetVolume(double x)
{
}

void AudioSource::RegisterAudioObserver(AudioObserver* audio_observer)
{
}

void AudioSource::UnregisterAudioObserver(AudioObserver* audio_observer)
{
}

void AudioSource::AddSink(webrtc::AudioTrackSinkInterface* audio_track_sink_interface)
{
    std::cout << __func__ << std::endl;
    this->sink_ = audio_track_sink_interface;
}

void AudioSource::RemoveSink(webrtc::AudioTrackSinkInterface* audio_track_sink_interface)
{
    std::cout << __func__ << std::endl;
    this->sink_ = nullptr;
}

const cricket::AudioOptions AudioSource::options() const
{
    return AudioSourceInterface::options();
}

AudioSource::~AudioSource()
{
}
