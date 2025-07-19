//
// Created by bimba on 7/18/25.
//

#include "conductor.hpp"
#include "signaling_client_interface.hpp"
#include "peer_connection.hpp"


Conductor::Conductor():ref_count_(0)
{
}

void Conductor::set_peer_connection(PeerConnection* peer_connection)
{
    this->peer_connection_ = peer_connection;
}

void Conductor::set_signaling_client(SignalingClientInterface* signaling_client)
{
    this->signaling_client_ = signaling_client;
}

void Conductor::send_message(const std::string& message)
{
    this->signaling_client_->send(message);
}

void Conductor::handle_message(const std::string& message) {
    try {
        nlohmann::json json = nlohmann::json::parse(message);
        if (!json.contains("type")) {
            return;
        }

        std::string type = json.at("type").get<std::string>();
        if (type == "offer") {
            if (!json.contains("sdp")) {
                return;
            }

            if (!peer_connection_) {
                return;
            }

            this->peer_connection_->handle_offer(json.at("sdp").get<std::string>());
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
        } else {
        }
    } catch (const std::exception& e) {
    }
}
