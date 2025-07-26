//
// Created by bimba on 27.06.25.
//

#include "video_track_source.hpp"

void VideoTrackSource::sendFrame(const webrtc::VideoFrame& frame)
{
    if (!this->sink_)
    {
        return;
    }

    this->sink_->OnFrame(frame);
}

void VideoTrackSource::AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink,
                                       const rtc::VideoSinkWants &wants) {
    this->sink_ = sink;
}

void VideoTrackSource::RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink) {
    this->sink_ = nullptr;
}

void VideoTrackSource::RequestRefreshFrame() {
    VideoTrackSourceInterface::RequestRefreshFrame();
}

void VideoTrackSource::RegisterObserver(webrtc::ObserverInterface *observer) {
}

void VideoTrackSource::UnregisterObserver(webrtc::ObserverInterface *observer) {
}

webrtc::MediaSourceInterface::SourceState VideoTrackSource::state() const {
    return webrtc::MediaSourceInterface::SourceState::kLive;
}

bool VideoTrackSource::remote() const {
    return false;
}

bool VideoTrackSource::is_screencast() const {
    return false;
}

std::optional<bool> VideoTrackSource::needs_denoising() const {
    return std::nullopt;
}

bool VideoTrackSource::GetStats(Stats *stats) {
    return false;
}

bool VideoTrackSource::SupportsEncodedOutput() const {
    return false;
}

void VideoTrackSource::GenerateKeyFrame() {
}

void VideoTrackSource::AddEncodedSink(rtc::VideoSinkInterface<webrtc::RecordableEncodedFrame> *sink) {
}

void VideoTrackSource::RemoveEncodedSink(rtc::VideoSinkInterface<webrtc::RecordableEncodedFrame> *sink) {
}

void VideoTrackSource::ProcessConstraints(const webrtc::VideoTrackSourceConstraints &video_track_source_constraints) {
    VideoTrackSourceInterface::ProcessConstraints(video_track_source_constraints);
}

VideoTrackSource::~VideoTrackSource() {
}