//
// Created by bimba on 27.06.25.
//

#include "audio_source.hpp"


void AudioSource::RegisterObserver(webrtc::ObserverInterface *observer) {
}

void AudioSource::UnregisterObserver(webrtc::ObserverInterface *observer) {
}

webrtc::MediaSourceInterface::SourceState AudioSource::state() const {
    return webrtc::MediaSourceInterface::SourceState::kLive;
}

bool AudioSource::remote() const {
}

void AudioSource::SetVolume(double x) {
    AudioSourceInterface::SetVolume(x);
}

void AudioSource::RegisterAudioObserver(AudioObserver *audio_observer) {
    AudioSourceInterface::RegisterAudioObserver(audio_observer);
}

void AudioSource::UnregisterAudioObserver(AudioObserver *audio_observer) {
    AudioSourceInterface::UnregisterAudioObserver(audio_observer);
}

void AudioSource::AddSink(webrtc::AudioTrackSinkInterface *audio_track_sink_interface) {
    AudioSourceInterface::AddSink(audio_track_sink_interface);
}

void AudioSource::RemoveSink(webrtc::AudioTrackSinkInterface *audio_track_sink_interface) {
    AudioSourceInterface::RemoveSink(audio_track_sink_interface);
}

const webrtc::AudioOptions AudioSource::options() const {
    return AudioSourceInterface::options();
}

AudioSource::~AudioSource() {
}
