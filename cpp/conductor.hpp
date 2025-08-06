//
// Created by bimba on 7/18/25.
//

#ifndef CONDUCTOR_H
#define CONDUCTOR_H
#include "audio_feeder.hpp"
#include "spdlog/spdlog.h"

class RtcClient;
class SignalingClientInterface;

#include <string>
#include <mutex>

#include "pc/webrtc_session_description_factory.h"
#include "absl/types/optional.h"

class DummySetSessionDescriptionObserver
    : public webrtc::SetSessionDescriptionObserver
{
public:
    static rtc::scoped_refptr<DummySetSessionDescriptionObserver> Create()
    {
        return rtc::make_ref_counted<DummySetSessionDescriptionObserver>();
    }

    virtual void OnSuccess()
    {
        RTC_LOG(LS_INFO) << __FUNCTION__;
    }

    virtual void OnFailure(webrtc::RTCError error)
    {
        RTC_LOG(LS_INFO) << __FUNCTION__ << " " << ToString(error.type()) << ": "
            << error.message();
    }
};

class Conductor
{
public:
    Conductor();

    void set_signaling_client(SignalingClientInterface* signaling_client);
    void set_audio_feeder(const std::shared_ptr<AudioFeeder>& audio_feeder);

    void send_message(const std::string& message);
    void handle_message(const std::string& message);

private:
    void setup_logger_();
    std::map<std::string, webrtc::scoped_refptr<RtcClient>> clients_;
    SignalingClientInterface* signaling_client_;
    std::shared_ptr<AudioFeeder> audio_feeder_;

    mutable int ref_count_;
    mutable std::mutex ref_mutex_;

    std::shared_ptr<spdlog::logger> logger_;
};


#endif //CONDUCTOR_H
