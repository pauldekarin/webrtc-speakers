//
// Created by bimba on 8/6/25.
//

#ifndef DUMMY_RTC_CLIENT_H
#define DUMMY_RTC_CLIENT_H
#include "api/peer_connection_interface.h"


class DummyRTCClient : public webrtc::PeerConnectionObserver, public webrtc::CreateSessionDescriptionObserver
{
public:
    void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
    void OnFailure(webrtc::RTCError error) override;

protected:
    ~DummyRTCClient() override;

public:
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
};


#endif //DUMMY_RTC_CLIENT_H
