//
// Created by bimba on 27.06.25.
//

#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H


#include <api/call/audio_sink.h>

#include "webrtc_includes.h"

class AudioSource: public webrtc::AudioSourceInterface {
public:
    void send(const void *audio_data, int bits_per_sample, int sample_rate, size_t number_of_channels, size_t number_of_frames);

    void silence();

    void RegisterObserver(webrtc::ObserverInterface *observer) override;

    void UnregisterObserver(webrtc::ObserverInterface *observer) override;

    SourceState state() const override;

    bool remote() const override;

    void SetVolume(double) override;

    void RegisterAudioObserver(AudioObserver *) override;

    void UnregisterAudioObserver(AudioObserver *) override;

    void AddSink(webrtc::AudioTrackSinkInterface *) override;

    void RemoveSink(webrtc::AudioTrackSinkInterface *) override;

    const cricket::AudioOptions options() const override;

    ~AudioSource() override;

private:
    webrtc::AudioTrackSinkInterface *sink_ = nullptr;
};



#endif //AUDIO_SOURCE_H
