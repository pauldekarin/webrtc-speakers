//
// Created by bimba on 7/18/25.
//

#include "conductor.hpp"
#include "signaling_client_interface.hpp"
#include "rtc_client.hpp"


Conductor::Conductor(): ref_count_(0)
{
    this->setup_logger_();
}

void Conductor::set_signaling_client(SignalingClientInterface* signaling_client)
{
    this->signaling_client_ = signaling_client;
}

void Conductor::set_audio_feeder(const std::shared_ptr<AudioFeeder>& audio_feeder)
{
    this->audio_feeder_ = audio_feeder;
}

void Conductor::send_message(const std::string& message)
{
    if (this->signaling_client_)
    {
        this->signaling_client_->send(message);
    }
    else
    {
        std::cerr << "Failed to send message. Signaling client is not set!" << std::endl;
    }
}

void Conductor::handle_message(const std::string& message)
{
    this->logger_->info("Parsing message");

    nlohmann::json json = nlohmann::json::parse(message);
    if (!json.contains("type"))
    {
        this->logger_->error("Failed to parse message. Received undefined type of message");

        throw std::runtime_error("Failed to parse message. Received undefined type of message");
    }

    std::string type = json.at("type").get<std::string>();

    this->logger_->info("Received '{}' message", type);

    if (type == "offer")
    {
        if (!json.contains("client_id"))
        {
            this->logger_->error(
                "Did not received `client_id`, so cannot handle offer Since further communication is not possible with this identifier");

            throw std::runtime_error("Undefined client_id");
        }

        std::string client_id = json.at("client_id").get<std::string>();

        if (!json.contains("sdp"))
        {
            this->logger_->error("Received message without SDP");

            throw new std::runtime_error("Received message without SDP");
        }

        webrtc::scoped_refptr<RtcClient> client = webrtc::make_ref_counted<RtcClient>(client_id, this);
        client->create_peer_connection();
        client->handle_offer(json.at("sdp").get<std::string>());
        client->AddRef();

        this->clients_.insert({client_id, client});
        this->audio_feeder_->add_sink(client.get());
    }
    else if (type == "candidate")
    {
        if (!json.contains("client_id"))
        {
            this->logger_->error(
                "Did not received `client_id`, so cannot handle offer Since further communication is not possible with this identifier");

            throw std::runtime_error("Undefined client_id");
        }

        std::string client_id = json.at("client_id").get<std::string>();

        if (this->clients_.find(client_id) == this->clients_.end())
        {
            this->logger_->error("Unknow client id");

            return;
        }

        webrtc::scoped_refptr<RtcClient> client = this->clients_.at(client_id);
        client->handle_candidate(json);
    }
}

void Conductor::setup_logger_()
{
    this->logger_ = spdlog::create<log_sink>("Condcutor");
}
