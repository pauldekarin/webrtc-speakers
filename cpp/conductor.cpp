//
// Created by bimba on 7/18/25.
//

#include "conductor.hpp"
#include "signaling_client.hpp"
#include "peer_connection.hpp"

void DummySetSessionDescriptionObserver::OnSuccess() {
}
void DummySetSessionDescriptionObserver::OnFailure(webrtc::RTCError error) {
}

rtc::scoped_refptr<DummySetSessionDescriptionObserver>  DummySetSessionDescriptionObserver::Create()
{
    return rtc::make_ref_counted<DummySetSessionDescriptionObserver>();
}

Conductor::Conductor():ref_count_(0)
{
}

void Conductor::set_peer_connection(PeerConnection* peer_connection)
{
    this->peer_connection_ = peer_connection;
}

void Conductor::set_signaling_client(SignalingClient* signaling_client)
{
    this->signaling_client_ = signaling_client;
}

void Conductor::send_message(const std::string& message)
{
    this->signaling_client_->send(message);
}

void Conductor::handle_message(const std::string& message) {
    try {
        lwsl_user("%s %s", __func__, message.c_str());
        nlohmann::json json = nlohmann::json::parse(message);
        if (!json.contains("type")) {
            lwsl_err("Message does not contain 'type' field\n");
            return;
        }

        std::string type = json.at("type").get<std::string>();
        if (type == "offer") {
            if (!json.contains("sdp")) {
                lwsl_err("Offer message does not contain 'sdp' field\n");
                return;
            }

            if (!peer_connection_) {
                lwsl_err("Cannot handle offer: peer_connection_ is null\n");
                return;
            }

            auto* pc = peer_connection_->get_pc();
            if (!pc) {
                lwsl_err("Cannot handle offer: get_pc() returned null\n");
                return;
            }

            const std::string raw_sdp = json.at("sdp").get<std::string>();
            const absl::optional<webrtc::SdpType> sdp_type = webrtc::SdpTypeFromString(type);
            if (!sdp_type) {
                lwsl_err("Invalid SDP type: %s\n", type.c_str());
                return;
            }

            webrtc::SdpParseError error;
            std::unique_ptr<webrtc::SessionDescriptionInterface> remote_sdp =
                webrtc::CreateSessionDescription(*sdp_type, raw_sdp, &error);
            if (!remote_sdp) {
                lwsl_err("Failed to create session description: %s\n", error.description.c_str());
                return;
            }

            // Log before calling SetRemoteDescription
            lwsl_user("peer_connection_: %p, get_pc(): %p, remote_sdp: %p\n",
                      peer_connection_, pc, remote_sdp.get());
            lwsl_user("Calling SetRemoteDescription on signaling thread: %ld\n",
                      std::this_thread::get_id());

            pc->SetRemoteDescription(DummySetSessionDescriptionObserver::Create().get(), remote_sdp.release());
            pc->CreateAnswer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
        } else if (type == "answer") {
            // std::string raw_sdp = json.at("sdp").get<std::string>();
            // webrtc::SdpParseError error;
            // absl::optional<webrtc::SdpType> sdp_type = webrtc::SdpTypeFromString(type);
            // std::unique_ptr<webrtc::SessionDescriptionInterface> remote_sdp = webrtc::CreateSessionDescription(*sdp_type, raw_sdp, &error);
            //
            // if (!remote_sdp)
            // {
            //     lwsl_err("Failed to create session description: %s\n", error.description.c_str());
            //     return;
            // }
            //
            // this->peer_connection_->get_pc()->SetRemoteDescription(this, remote_sdp.release());
        } else if (type == "candidate") {
            // TODO: Handle candidate
        } else {
            lwsl_err("Unknown message type: %s\n", type.c_str());
        }
    } catch (const std::exception& e) {
        lwsl_err("Exception in handle_message: %s\n", e.what());
    }
}

void Conductor::AddRef() const
{
    std::unique_lock lock(this->ref_mutex_);
    this->ref_count_++;

}

webrtc::RefCountReleaseStatus Conductor::Release() const
{
    std::unique_lock lock(this->ref_mutex_);
    if (--this->ref_count_ <= 0)
    {
        return webrtc::RefCountReleaseStatus::kDroppedLastRef;
    }

    return webrtc::RefCountReleaseStatus::kOtherRefsRemained;
}

void Conductor::OnSuccess(webrtc::SessionDescriptionInterface* desc)
{
    lwsl_user("%s %s", __func__, "get signal");
    // this->peer_connection_->get_pc()->SetLocalDescription(DummySetSessionDescriptionObserver::Create().get(), desc);
    //
    std::string type = webrtc::SdpTypeToString(desc->GetType());
    std::string sdp;
    desc->ToString(&sdp);

    nlohmann::json offer = {
        {"type",type},
        {"sdp",sdp}
    };

    if (this->signaling_client_ != nullptr)
    {
        this->signaling_client_->send(offer.dump());
    }
}

void Conductor::OnFailure(webrtc::RTCError error)
{
    std::cout << "ERROR " << error.message() << std::endl;
}

Conductor::~Conductor()
{
}

void Conductor::OnSuccess()
{
}

webrtc::CreateSessionDescriptionObserver* Conductor::get_create_sdp_observer()
{
    return this;
}
