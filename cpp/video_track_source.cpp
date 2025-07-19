//
// Created by bimba on 27.06.25.
//

#include "video_track_source.hpp"

void VideoTrackSource::AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink,
    const rtc::VideoSinkWants &wants) {
}

void VideoTrackSource::RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink) {
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
}

bool VideoTrackSource::is_screencast() const {
}

std::optional<bool> VideoTrackSource::needs_denoising() const {
}

bool VideoTrackSource::GetStats(Stats *stats) {
}

bool VideoTrackSource::SupportsEncodedOutput() const {
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