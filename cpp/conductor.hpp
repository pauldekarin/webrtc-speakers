//
// Created by bimba on 7/18/25.
//

#ifndef CONDUCTOR_H
#define CONDUCTOR_H

class PeerConnection;
class SignalingClient;

#include <string>
#include <mutex>

#include "pc/webrtc_session_description_factory.h"
#include "absl/types/optional.h"

class DummySetSessionDescriptionObserver: public webrtc::SetSessionDescriptionObserver
{
public:
    void OnSuccess() override;
    void OnFailure(webrtc::RTCError error) override;

    static rtc::scoped_refptr<DummySetSessionDescriptionObserver> Create();
};

class Conductor: public webrtc::CreateSessionDescriptionObserver, public webrtc::SetSessionDescriptionObserver{
public:
    Conductor();

    void set_peer_connection(PeerConnection *peer_connection);
    void set_signaling_client(SignalingClient *signaling_client);

    void send_message(const std::string &message);
    void handle_message(const std::string &message);

    void AddRef() const override;
    webrtc::RefCountReleaseStatus Release() const override;
    void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
    void OnFailure(webrtc::RTCError error) override;

    webrtc::CreateSessionDescriptionObserver* get_create_sdp_observer();

protected:
    ~Conductor() override;

public:
    void OnSuccess() override;

private:
    SignalingClient* signaling_client_;
    PeerConnection* peer_connection_;
    mutable int ref_count_;
    mutable std::mutex ref_mutex_;
};



#endif //CONDUCTOR_H
