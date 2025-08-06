//
// Created by bimba on 8/6/25.
//

#include "dummy_rtc_client.hpp"


void DummyRTCClient::OnSuccess(webrtc::SessionDescriptionInterface* desc)
{
}

void DummyRTCClient::OnFailure(webrtc::RTCError error)
{
}

DummyRTCClient::~DummyRTCClient()
{
}

void DummyRTCClient::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{
}

void DummyRTCClient::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> media_stream_interface)
{
    PeerConnectionObserver::OnAddStream(media_stream_interface);
}

void DummyRTCClient::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> media_stream_interface)
{
    PeerConnectionObserver::OnRemoveStream(media_stream_interface);
}

void DummyRTCClient::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel)
{
}

void DummyRTCClient::OnRenegotiationNeeded()
{
    PeerConnectionObserver::OnRenegotiationNeeded();
}

void DummyRTCClient::OnNegotiationNeededEvent(uint32_t uint32)
{
    PeerConnectionObserver::OnNegotiationNeededEvent(uint32);
}

void DummyRTCClient::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState ice_connection_state)
{
    PeerConnectionObserver::OnIceConnectionChange(ice_connection_state);
}

void DummyRTCClient::OnStandardizedIceConnectionChange(
    webrtc::PeerConnectionInterface::IceConnectionState ice_connection_state)
{
    PeerConnectionObserver::OnStandardizedIceConnectionChange(ice_connection_state);
}

void DummyRTCClient::OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState peer_connection_state)
{
    PeerConnectionObserver::OnConnectionChange(peer_connection_state);
}

void DummyRTCClient::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
}

void DummyRTCClient::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
}

void DummyRTCClient::OnIceCandidateError(const std::string& basic_string, int i, const std::string& string, int i1,
                                         const std::string& basic_string1)
{
    PeerConnectionObserver::OnIceCandidateError(basic_string, i, string, i1, basic_string1);
}

void DummyRTCClient::OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates)
{
    PeerConnectionObserver::OnIceCandidatesRemoved(candidates);
}

void DummyRTCClient::OnIceConnectionReceivingChange(bool cond)
{
    PeerConnectionObserver::OnIceConnectionReceivingChange(cond);
}

void DummyRTCClient::OnIceSelectedCandidatePairChanged(
    const cricket::CandidatePairChangeEvent& candidate_pair_change_event)
{
    PeerConnectionObserver::OnIceSelectedCandidatePairChanged(candidate_pair_change_event);
}

void DummyRTCClient::OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver_interface,
                                const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&
                                media_stream_interfaces)
{
    PeerConnectionObserver::OnAddTrack(rtp_receiver_interface, media_stream_interfaces);
}

void DummyRTCClient::OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> rtp_transceiver_interface)
{
    PeerConnectionObserver::OnTrack(rtp_transceiver_interface);
}

void DummyRTCClient::OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver_interface)
{
    PeerConnectionObserver::OnRemoveTrack(rtp_receiver_interface);
}

void DummyRTCClient::OnInterestingUsage(int i)
{
    PeerConnectionObserver::OnInterestingUsage(i);
}
