//
// Created by bimba on 27.06.25.
//

#ifndef PEER_CONNECTION_H
#define PEER_CONNECTION_H

#include "nlohmann/json.hpp"
#include "webrtc_includes.h"
#include "audio_capture_module.hpp"
#include "audio_handler_interface.hpp"
#include "audio_source.hpp"
#include "dummy_rtc_client.hpp"
#include "video_track_source.hpp"
#include "settings.hpp"

class Conductor;

class AudioReceiver : public webrtc::AudioTrackSinkInterface
{
public:
    AudioSource* sink_;

    AudioReceiver(AudioSource* sink);

    void OnData(const void* audio_data, int bits_per_sample, int sample_rate, size_t number_of_channels,
                size_t number_of_frames) override;
};

class VideoReceiver : public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
    VideoTrackSource* sink_;

    VideoReceiver(VideoTrackSource* sink);

    void OnFrame(const webrtc::VideoFrame& frame) override;
};

class RtcClient : public DummyRTCClient,
                  public AudioHandlerInterface
{
public:
    rtc::scoped_refptr<AudioSource> audio_track_source_;
    rtc::scoped_refptr<VideoTrackSource> video_track_source_;

    ~RtcClient();
    RtcClient(const std::string& id, Conductor* conductor = nullptr);

    void create_peer_connection();
    void handle_offer(const std::string& sdp);
    void handle_candidate(const nlohmann::json& data);
    void send_json(nlohmann::json& data);

    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override;
    void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface>) override;
    void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface>) override;
    void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
    void OnRenegotiationNeeded() override;
    void OnNegotiationNeededEvent(uint32_t) override;
    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState) override;
    void OnStandardizedIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState) override;
    void OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState) override;
    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
    void OnIceCandidateError(const std::string&, int, const std::string&, int, const std::string&) override;
    void OnIceCandidatesRemoved(const std::vector<cricket::Candidate>&) override;
    void OnIceConnectionReceivingChange(bool) override;
    void OnIceSelectedCandidatePairChanged(const cricket::CandidatePairChangeEvent&) override;
    void OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface>,
                    const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&) override;
    void OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface>) override;
    void OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface>) override;
    void OnInterestingUsage(int) override;
    void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
    void OnFailure(webrtc::RTCError error) override;
    void handle(const void* audio_data, int bits_per_sample, int sample_rate, size_t number_of_channels,
                size_t number_of_frames) override;

private:
    void setup_logger_();
    std::vector<webrtc::scoped_refptr<webrtc::PeerConnectionInterface>> peer_connections_;

    std::unique_ptr<rtc::Thread> signaling_thread_;
    std::unique_ptr<AudioReceiver> audio_receiver_;
    std::unique_ptr<VideoReceiver> video_receiver_;
    webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
    std::string id_;
    Conductor* conductor_;

    const std::string class_name_ = "RtcClient";
    std::shared_ptr<spdlog::logger> logger_;
};


#endif //PEER_CONNECTION_H
