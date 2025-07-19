//
// Created by bimba on 27.06.25.
//

#include "peer_connection.hpp"
#include "conductor.hpp"

PeerConnection::PeerConnection(Conductor *conductor):
conductor_(conductor)
{
}

PeerConnection::~PeerConnection()
{

}


void PeerConnection::call(){

    this->signaling_thread_ = rtc::Thread::CreateWithSocketServer();
    this->signaling_thread_->Start();

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

    if(factory){
        webrtc::PeerConnectionInterface::RTCConfiguration configuration;
        webrtc::PeerConnectionDependencies dependencies(this);

        auto result = factory->CreatePeerConnectionOrError(configuration, std::move(dependencies));

        if (!result.ok()) {
            std::cerr << "Failed to create PeerConnection or error" << std::endl;
            return;
        }

        this->peer_connection_= std::move(result.value());

        webrtc::scoped_refptr<AudioSource> audio_track_source = webrtc::make_ref_counted<AudioSource>();
        webrtc::scoped_refptr<VideoTrackSource> video_track_source = webrtc::make_ref_counted<VideoTrackSource>();

        rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(factory->CreateAudioTrack("audio", audio_track_source.get()));
        rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(factory->CreateVideoTrack("video", video_track_source.get()));

        auto r = this->peer_connection_->AddTrack(audio_track, {"stream-id"});
        this->peer_connection_->AddTrack(video_track, {"stream-id"});

        if (!r.ok()) {
            std::cerr << "Failed to add Audio Track" << std::endl;
        }

        // this->peer_connection_->CreateOffer(this->conductor_->get_create_sdp_observer(), webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());

      }else {
          std::cerr << "Failed to create PeerConnectionFactory" << std::endl;
      }

    std::cout << "called" << std::endl;
  }

void PeerConnection::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{
}

void PeerConnection::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> media_stream_interface)
{
    PeerConnectionObserver::OnAddStream(media_stream_interface);
}

void PeerConnection::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> media_stream_interface)
{
    PeerConnectionObserver::OnRemoveStream(media_stream_interface);
}

void PeerConnection::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel)
{
}

void PeerConnection::OnRenegotiationNeeded()
{
    PeerConnectionObserver::OnRenegotiationNeeded();
}

void PeerConnection::OnNegotiationNeededEvent(uint32_t uint32)
{
    PeerConnectionObserver::OnNegotiationNeededEvent(uint32);
}

void PeerConnection::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState ice_connection_state)
{
    PeerConnectionObserver::OnIceConnectionChange(ice_connection_state);
}

void PeerConnection::OnStandardizedIceConnectionChange(
    webrtc::PeerConnectionInterface::IceConnectionState ice_connection_state)
{
    PeerConnectionObserver::OnStandardizedIceConnectionChange(ice_connection_state);
}

void PeerConnection::OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState peer_connection_state)
{
    PeerConnectionObserver::OnConnectionChange(peer_connection_state);
}

void PeerConnection::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
}

void PeerConnection::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
}

void PeerConnection::OnIceCandidateError(const std::string& basic_string, int i, const std::string& string, int i1,
    const std::string& basic_string1)
{
    PeerConnectionObserver::OnIceCandidateError(basic_string, i, string, i1, basic_string1);
}

void PeerConnection::OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates)
{
    PeerConnectionObserver::OnIceCandidatesRemoved(candidates);
}

void PeerConnection::OnIceConnectionReceivingChange(bool cond)
{
    PeerConnectionObserver::OnIceConnectionReceivingChange(cond);
}

void PeerConnection::OnIceSelectedCandidatePairChanged(
    const cricket::CandidatePairChangeEvent& candidate_pair_change_event)
{
    PeerConnectionObserver::OnIceSelectedCandidatePairChanged(candidate_pair_change_event);
}

void PeerConnection::OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver_interface,
    const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>& media_stream_interfaces)
{
    PeerConnectionObserver::OnAddTrack(rtp_receiver_interface, media_stream_interfaces);
}

void PeerConnection::OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> rtp_transceiver_interface)
{
    PeerConnectionObserver::OnTrack(rtp_transceiver_interface);
}

void PeerConnection::OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver_interface)
{
    PeerConnectionObserver::OnRemoveTrack(rtp_receiver_interface);
}

void PeerConnection::OnInterestingUsage(int i)
{
    PeerConnectionObserver::OnInterestingUsage(i);
}

webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> createFactory(){
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

            if(!factory){
                  factory->AddRef();

                  return factory;
              }

            return nullptr;
        }
