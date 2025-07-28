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

    std::cout << "bits per sample: " << bits_per_sample <<  " sample_rate " << sample_rate << " channels " << number_of_channels << " frames " << number_of_frames <<std::endl;
    this->sink_->OnData(audio_data, bits_per_sample, sample_rate, number_of_channels, number_of_frames);
}

void AudioSource::silence()
{
    if (!this->sink_)
    {
        return;
    }
    const int bits_per_sample = 16; // Common for PCM audio (16-bit samples)
    const int sample_rate = 44100;  // Common sample rate (44.1 kHz)
    const size_t number_of_channels = 2; // Stereo
    const size_t number_of_frames = 1024; // Typical frame size (adjust as needed)

    // Calculate the size of the silence buffer in bytes
    // For 16-bit audio: bytes = number_of_frames * number_of_channels * (bits_per_sample / 8)
    const size_t buffer_size = number_of_frames * number_of_channels * (bits_per_sample / 8);

    // Create a buffer filled with zeros (silence)
    std::vector<uint8_t> silence_buffer(buffer_size, 0);

    // Send the silence buffer to the sink
    this->sink_->OnData(silence_buffer.data(), bits_per_sample, sample_rate,
                        number_of_channels, number_of_frames);
}

void AudioSource::RegisterObserver(webrtc::ObserverInterface *observer) {
}

void AudioSource::UnregisterObserver(webrtc::ObserverInterface *observer) {
}

webrtc::MediaSourceInterface::SourceState AudioSource::state() const {
    return webrtc::MediaSourceInterface::SourceState::kLive;
}

bool AudioSource::remote() const {
    return false;
}

void AudioSource::SetVolume(double x) {
}

void AudioSource::RegisterAudioObserver(AudioObserver *audio_observer) {
}

void AudioSource::UnregisterAudioObserver(AudioObserver *audio_observer) {
}

void AudioSource::AddSink(webrtc::AudioTrackSinkInterface *audio_track_sink_interface) {
    std::cout << __func__ << std::endl;
    this->sink_ = audio_track_sink_interface;
}

void AudioSource::RemoveSink(webrtc::AudioTrackSinkInterface *audio_track_sink_interface) {
    std::cout << __func__ << std::endl;
    this->sink_ = nullptr;
}

const cricket::AudioOptions AudioSource::options() const {
    return AudioSourceInterface::options();
}

AudioSource::~AudioSource() {
}
