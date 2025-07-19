//
// Created by bimba on 7/18/25.
//

#ifndef CONDUCTOR_H
#define CONDUCTOR_H

class PeerConnection;
class SignalingClientInterface;

#include <string>
#include <mutex>

#include "pc/webrtc_session_description_factory.h"
#include "absl/types/optional.h"

class DummySetSessionDescriptionObserver
    : public webrtc::SetSessionDescriptionObserver {
public:
    static rtc::scoped_refptr<DummySetSessionDescriptionObserver> Create() {
        return rtc::make_ref_counted<DummySetSessionDescriptionObserver>();
    }
    virtual void OnSuccess() { RTC_LOG(LS_INFO) << __FUNCTION__; }
    virtual void OnFailure(webrtc::RTCError error) {
        RTC_LOG(LS_INFO) << __FUNCTION__ << " " << ToString(error.type()) << ": "
                         << error.message();
    }
};

class Conductor{
public:
    Conductor();

    void set_peer_connection(PeerConnection *peer_connection);
    void set_signaling_client(SignalingClientInterface *signaling_client);

    void send_message(const std::string &message);
    void handle_message(const std::string &message);

private:
    SignalingClientInterface* signaling_client_;
    PeerConnection* peer_connection_;
    mutable int ref_count_;
    mutable std::mutex ref_mutex_;
};



#endif //CONDUCTOR_H
