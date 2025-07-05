//
// Created by bimba on 27.06.25.
//

#ifndef VIDEO_TRACK_SOURCE_H
#define VIDEO_TRACK_SOURCE_H


#include "webrtc_includes.h"

class VideoTrackSource: public webrtc::VideoTrackSourceInterface {
public:
    void AddOrUpdateSink(webrtc::VideoSinkInterface<webrtc::VideoFrame> *sink,
        const webrtc::VideoSinkWants &wants) override;

    void RemoveSink(webrtc::VideoSinkInterface<webrtc::VideoFrame> *sink) override;

    void RequestRefreshFrame() override;

    void RegisterObserver(webrtc::ObserverInterface *observer) override;

    void UnregisterObserver(webrtc::ObserverInterface *observer) override;

    SourceState state() const override;

    bool remote() const override;

    bool is_screencast() const override;

    std::optional<bool> needs_denoising() const override;

    bool GetStats(Stats *stats) override;

    bool SupportsEncodedOutput() const override;

    void GenerateKeyFrame() override;

    void AddEncodedSink(webrtc::VideoSinkInterface<webrtc::RecordableEncodedFrame> *sink) override;

    void RemoveEncodedSink(webrtc::VideoSinkInterface<webrtc::RecordableEncodedFrame> *sink) override;

    void ProcessConstraints(const webrtc::VideoTrackSourceConstraints &) override;

protected:
    ~VideoTrackSource() override;
};


#endif //VIDEO_TRACK_SOURCE_H
