//
// Created by bimba on 27.06.25.
//

#include "peer_connection.hpp"

#include "spdlog/spdlog.h"

void AudioReceiver::OnData(const void* audio_data, int bits_per_sample, int sample_rate, size_t number_of_channels,
                           size_t number_of_frames)
{
    if (!this->sink_)
    {
        return;
    }

    this->sink_->send(audio_data, bits_per_sample, sample_rate, number_of_channels, number_of_frames);
}

#include "conductor.hpp"

AudioReceiver::AudioReceiver(AudioSource* sink):
    sink_(sink)
{
}

VideoReceiver::VideoReceiver(VideoTrackSource* sink):
    sink_(sink)
{
}

void VideoReceiver::OnFrame(const webrtc::VideoFrame& frame)
{
    if (!this->sink_)
    {
        return;
    }

    this->sink_->sendFrame(frame);
}


PeerConnection::PeerConnection(Conductor* conductor):
    conductor_(conductor)
{
    this->setup_logger_();
}

PeerConnection::~PeerConnection()
{
}


void PeerConnection::create_peer_connection()
{
    this->logger_->info("Setting up signaling thread");
    this->signaling_thread_ = rtc::Thread::CreateWithSocketServer();
    this->signaling_thread_->Start();

    // webrtc::scoped_refptr<webrtc::AudioDeviceModule> audio_device = webrtc::AudioDeviceModule::Create(webrtc::AudioDeviceModule::kPlatformDefaultAudio, nullptr);

    this->logger_->info("Creating factory");
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory = webrtc::CreatePeerConnectionFactory(
        nullptr, nullptr, this->signaling_thread_.get(),
        webrtc::scoped_refptr<webrtc::AudioDeviceModule>(webrtc::make_ref_counted<AudioCaptureModule>()),
        webrtc::CreateBuiltinAudioEncoderFactory(),
        webrtc::CreateBuiltinAudioDecoderFactory(),
        std::make_unique<webrtc::VideoEncoderFactoryTemplate<
            webrtc::LibvpxVp8EncoderTemplateAdapter,
            webrtc::LibvpxVp9EncoderTemplateAdapter,
            webrtc::OpenH264EncoderTemplateAdapter,
            webrtc::LibaomAv1EncoderTemplateAdapter>>(),
        std::make_unique<webrtc::VideoDecoderFactoryTemplate<
            webrtc::LibvpxVp8DecoderTemplateAdapter,
            webrtc::LibvpxVp9DecoderTemplateAdapter,
            webrtc::OpenH264DecoderTemplateAdapter,
            webrtc::Dav1dDecoderTemplateAdapter>>(),
        nullptr /* audio_mixer */, nullptr);

    if (factory)
    {
        webrtc::PeerConnectionInterface::RTCConfiguration configuration;
        webrtc::PeerConnectionDependencies dependencies(this);
        configuration.audio_jitter_buffer_max_packets = 0;

        auto result = factory->CreatePeerConnectionOrError(configuration, std::move(dependencies));

        if (!result.ok())
        {
            this->logger_->error("Failed to create peer connection");
            return;
        }

        this->peer_connection_ = std::move(result.value());
        this->audio_track_source_ = webrtc::make_ref_counted<AudioSource>();
        rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
            factory->CreateAudioTrack("audio", this->audio_track_source_.get()));

        auto r = this->peer_connection_->AddTrack(audio_track, {"stream-id"});
        if (!r.ok())
        {
            this->logger_->error("Failed to add audio track");
        }

        this->video_track_source_ = webrtc::make_ref_counted<VideoTrackSource>();
        webrtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
            factory->CreateVideoTrack(this->video_track_source_, "video"));

        auto f = this->peer_connection_->AddTrack(video_track, {"stream-id"});
        if (!f.ok())
        {
            this->logger_->error("Failed to add video track");
        }
        this->video_receiver_ = std::make_unique<VideoReceiver>(this->video_track_source_.get());
        // this->audio_receiver_ = std::make_unique<AudioReceiver>(this->audio_track_source_.get());
    }
    else
    {
        this->logger_->error("Failed to create PeerConnection or error");
    }
}

void PeerConnection::handle_offer(const std::string& sdp)
{
    const absl::optional<webrtc::SdpType> sdp_type = webrtc::SdpTypeFromString("offer");
    if (!sdp_type)
    {
        return;
    }

    webrtc::SdpParseError error;
    std::unique_ptr<webrtc::SessionDescriptionInterface> remote_sdp =
        webrtc::CreateSessionDescription(*sdp_type, sdp, &error);
    if (!remote_sdp)
    {
        return;
    }

    this->peer_connection_->SetRemoteDescription(DummySetSessionDescriptionObserver::Create().get(),
                                                 remote_sdp.release());
    this->peer_connection_->CreateAnswer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
}

void PeerConnection::handle_candidate(const nlohmann::json& data)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;

    int sdp_mlineindex = data.at("sdpMLineIndex").get<int>();
    std::string sdp_mid = data.at("sdpMid").get<std::string>();
    std::string sdp = data.at("candidate").get<std::string>();

    webrtc::SdpParseError error;
    std::unique_ptr<webrtc::IceCandidateInterface> candidate(
        webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp, &error));
    if (!candidate)
    {
        return;
    }

    if (!this->peer_connection_->AddIceCandidate(candidate.get()))
    {
        std::cerr << "failed to add IceCandidate" << std::endl;
    }

    std::cout << "added ice candidate" << std::endl;
}

void PeerConnection::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
}

void PeerConnection::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> media_stream_interface)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
    PeerConnectionObserver::OnAddStream(media_stream_interface);
}

void PeerConnection::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> media_stream_interface)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
    PeerConnectionObserver::OnRemoveStream(media_stream_interface);
}

void PeerConnection::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
}

void PeerConnection::OnRenegotiationNeeded()
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
    PeerConnectionObserver::OnRenegotiationNeeded();
}

void PeerConnection::OnNegotiationNeededEvent(uint32_t uint32)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
    PeerConnectionObserver::OnNegotiationNeededEvent(uint32);
}

void PeerConnection::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState ice_connection_state)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;

    PeerConnectionObserver::OnIceConnectionChange(ice_connection_state);
}

void PeerConnection::OnStandardizedIceConnectionChange(
    webrtc::PeerConnectionInterface::IceConnectionState ice_connection_state)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
    PeerConnectionObserver::OnStandardizedIceConnectionChange(ice_connection_state);
}

void PeerConnection::OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState peer_connection_state)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
    PeerConnectionObserver::OnConnectionChange(peer_connection_state);
}

void PeerConnection::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
}

void PeerConnection::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;

    if (!this->conductor_)
    {
        std::cerr << "undefined conductor" << std::endl;
        return;
    }

    std::string sdp;
    if (!candidate->ToString(&sdp))
    {
        std::cerr << "failed sdp toString()" << std::endl;
        return;
    }

    nlohmann::json data = {
        {"type", "candidate"},
        {"candidate", sdp},
        {"sdpMid", candidate->sdp_mid()},
        {"sdpMLineIndex", candidate->sdp_mline_index()},

    };

    this->conductor_->send_message(data.dump());
}

void PeerConnection::OnIceCandidateError(const std::string& basic_string, int i, const std::string& string, int i1,
                                         const std::string& basic_string1)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
    PeerConnectionObserver::OnIceCandidateError(basic_string, i, string, i1, basic_string1);
    std::cout << basic_string << std::endl;
    std::cout << i << std::endl;
    std::cout << string << std::endl;
    std::cout << i1 << std::endl;
    std::cout << basic_string1 << std::endl;
    std::cout << "=====" << __func__ << "=====" << std::endl;
}

void PeerConnection::OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
    PeerConnectionObserver::OnIceCandidatesRemoved(candidates);
}

void PeerConnection::OnIceConnectionReceivingChange(bool cond)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
    PeerConnectionObserver::OnIceConnectionReceivingChange(cond);
}

void PeerConnection::OnIceSelectedCandidatePairChanged(
    const cricket::CandidatePairChangeEvent& candidate_pair_change_event)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
    PeerConnectionObserver::OnIceSelectedCandidatePairChanged(candidate_pair_change_event);
}

void PeerConnection::OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver_interface,
                                const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&
                                media_stream_interfaces)
{
    std::cout << "=====OnAddTrack=====" << std::endl;
    auto* track = rtp_receiver_interface->track().release();
    if (this->audio_receiver_ && track->kind() == webrtc::MediaStreamTrackInterface::kAudioKind)
    {
        std::cout << "Added receiver to audio, track state: " << track->state()
            << ", enabled: " << (track->enabled() ? "true" : "false") << std::endl;
        auto* audio_track = static_cast<webrtc::AudioTrackInterface*>(track);
        audio_track->AddSink(this->audio_receiver_.get());
    }
    else if (this->video_receiver_ && track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind)
    {
        std::cout << "Added receiver to video, track state: " << track->state()
            << ", enabled: " << (track->enabled() ? "true" : "false") << std::endl;
        auto* video_track = static_cast<webrtc::VideoTrackInterface*>(track);
        video_track->AddOrUpdateSink(this->video_receiver_.get(), rtc::VideoSinkWants());
    }
    track->Release();
}

void PeerConnection::OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> rtp_transceiver_interface)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
    PeerConnectionObserver::OnTrack(rtp_transceiver_interface);
    std::cout << rtp_transceiver_interface->media_type() << std::endl;
    std::cout << rtp_transceiver_interface->stopped() << std::endl;
    std::cout << rtp_transceiver_interface->stopping() << std::endl;
    std::cout << "=====" << __func__ << "=====" << std::endl;
}

void PeerConnection::OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver_interface)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
    PeerConnectionObserver::OnRemoveTrack(rtp_receiver_interface);
}

void PeerConnection::OnInterestingUsage(int i)
{
    std::cout << "=====" << __func__ << "=====" << std::endl;
    PeerConnectionObserver::OnInterestingUsage(i);
}

void PeerConnection::OnSuccess(webrtc::SessionDescriptionInterface* desc)
{
    std::cout << "PeerConnection::OnSuccess" << std::endl;

    this->peer_connection_->SetLocalDescription(DummySetSessionDescriptionObserver::Create().get(), desc);

    std::string type = webrtc::SdpTypeToString(desc->GetType());
    std::string sdp;
    desc->ToString(&sdp);

    nlohmann::json offer = {
        {"type", type},
        {"sdp", sdp}
    };

    std::cout << offer.dump(4) << std::endl;

    if (this->conductor_ != nullptr)
    {
        this->conductor_->send_message(offer.dump());
    }
}

void PeerConnection::OnFailure(webrtc::RTCError error)
{
}

void PeerConnection::handle(const void* audio_data, int bits_per_sample, int sample_rate, size_t number_of_channels,
                            size_t number_of_frames)
{
    if (!this->audio_track_source_)
    {
        return;
    }

    this->audio_track_source_->send(audio_data, bits_per_sample, sample_rate, number_of_channels, number_of_frames);
}

void PeerConnection::setup_logger_()
{
    this->logger_ = spdlog::create<log_sink>(this->class_name_);
}

webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> createFactory()
{
    auto signaling_thread = rtc::Thread::CreateWithSocketServer();
    signaling_thread->Start();
    auto worker_thread = rtc::Thread::Create();
    worker_thread->Start();

    webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory = webrtc::CreatePeerConnectionFactory(
        nullptr, worker_thread.get(), signaling_thread.get(),
        webrtc::scoped_refptr<webrtc::AudioDeviceModule>(webrtc::make_ref_counted<AudioCaptureModule>()),
        webrtc::CreateBuiltinAudioEncoderFactory(),
        webrtc::CreateBuiltinAudioDecoderFactory(),
        std::make_unique<webrtc::VideoEncoderFactoryTemplate<
            webrtc::LibvpxVp8EncoderTemplateAdapter,
            webrtc::LibvpxVp9EncoderTemplateAdapter,
            webrtc::OpenH264EncoderTemplateAdapter,
            webrtc::LibaomAv1EncoderTemplateAdapter>>(),
        std::make_unique<webrtc::VideoDecoderFactoryTemplate<
            webrtc::LibvpxVp8DecoderTemplateAdapter,
            webrtc::LibvpxVp9DecoderTemplateAdapter,
            webrtc::OpenH264DecoderTemplateAdapter,
            webrtc::Dav1dDecoderTemplateAdapter>>(),
        nullptr /* audio_mixer */, nullptr);

    if (!factory)
    {
        factory->AddRef();

        return factory;
    }

    return nullptr;
}
