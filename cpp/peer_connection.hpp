//
// Created by bimba on 27.06.25.
//

#ifndef PEER_CONNECTION_H
#define PEER_CONNECTION_H

#include "nlohmann/json.hpp"
#include "webrtc_includes.h"
#include "audio_capture_module.hpp"
#include "audio_source.hpp"
#include "video_track_source.hpp"

class Conductor;

class SetSessionDescriptionObserver: public webrtc::SetSessionDescriptionObserver {
public:

    explicit  SetSessionDescriptionObserver(rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection = nullptr):peer_connection_(peer_connection) {
    }

    ~SetSessionDescriptionObserver() override = default;

    void OnSuccess() override {

    }

    void OnFailure(webrtc::RTCError error) override {

    }

private:
    webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
    mutable  int ref_count_ = 0;
};



class CreateSessionDescriptionObserver: public webrtc::CreateSessionDescriptionObserver {
public:
    CreateSessionDescriptionObserver(webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection):peer_connection_(peer_connection) {
    }

    void OnSuccess(webrtc::SessionDescriptionInterface *desc) override {
        webrtc::scoped_refptr<::SetSessionDescriptionObserver> observer = webrtc::make_ref_counted<::SetSessionDescriptionObserver>(this->peer_connection_);

        std::string type = webrtc::SdpTypeToString(desc->GetType());
        std::string sdp;
        desc->ToString(&sdp);

        this->peer_connection_->SetLocalDescription(observer.get(), desc);

        std::cout << sdp << std::endl;
    }

    void OnFailure(webrtc::RTCError error) override {

    }

private:
    webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
    int ref_count_;
};


class PeerConnection: public webrtc::PeerConnectionObserver{
protected:

public:
    ~PeerConnection();
    // void OnSuccess(webrtc::SessionDescriptionInterface* desc) override{

    //     std::string type = webrtc::SdpTypeToString(desc->GetType());
    //     std::string sdp;
    //     desc->ToString(&sdp);
    //
    //     nlohmann::json offer = {
    //         {"type","offer"},
    //         {"sdp",sdp.c_str()}
    //     };
    //
    //     this->signaling_client_->send(offer.dump());
    // }
    // void OnFailure(webrtc::RTCError error) override{
    //     std::cout << "OK";
    // }

    PeerConnection(Conductor *conductor);
    void call();
    webrtc::PeerConnectionInterface* get_pc()
    {
        return this->peer_connection_.get();
    }
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

private:
    std::unique_ptr<rtc::Thread> signaling_thread_;
    webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
    Conductor* conductor_;
};



#endif //PEER_CONNECTION_H
