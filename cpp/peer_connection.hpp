//
// Created by bimba on 27.06.25.
//

#ifndef PEER_CONNECTION_H
#define PEER_CONNECTION_H

#include "webrtc_includes.h"
#include "audio_capture_module.hpp"
#include "audio_source.hpp"
#include "video_track_source.hpp"

class PeerConnection;

class SetSessionDescriptionObserver: public webrtc::SetSessionDescriptionObserver {
public:

    explicit  SetSessionDescriptionObserver(PeerConnection* peer_connection = nullptr):peer_connection_(peer_connection) {
    }

    ~SetSessionDescriptionObserver() override {

    }

    void OnSuccess() override {

    }

    void OnFailure(webrtc::RTCError error) override {

    }

private:
    PeerConnection* peer_connection_;
    mutable  int ref_count_ = 0;
};



class CreateSessionDescriptionObserver: public webrtc::SetSessionDescriptionObserver {
public:
    CreateSessionDescriptionObserver(PeerConnection* peer_connection):peer_connection_(peer_connection) {

    }

    void OnSuccess() override {

    }

    void OnFailure(webrtc::RTCError error) override {

    }

private:
    PeerConnection* peer_connection_;
    int ref_count_;
};


class PeerConnection: public webrtc::PeerConnectionObserver, public webrtc::CreateSessionDescriptionObserver, public webrtc::SetSessionDescriptionObserver {
public:
    void AddRef() const override{
    }

    webrtc::RefCountReleaseStatus Release() const override{
        return webrtc::RefCountReleaseStatus::kDroppedLastRef;
    }

    void OnSuccess(webrtc::SessionDescriptionInterface *desc) override{
        webrtc::scoped_refptr<::SetSessionDescriptionObserver> observer = webrtc::make_ref_counted<::SetSessionDescriptionObserver>(this);

        std::string type = webrtc::SdpTypeToString(desc->GetType());
        std::string sdp;
        desc->ToString(&sdp);



        this->peer_connection_->SetLocalDescription(observer.get(), desc);
    }

    void OnFailure(webrtc::RTCError error) override{
        std::cout << "FAIL";
    }

    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override{
    }

    void OnAddStream(webrtc::scoped_refptr<webrtc::MediaStreamInterface>) override{
    }

    void OnRemoveStream(webrtc::scoped_refptr<webrtc::MediaStreamInterface>) override{
    }

    void OnDataChannel(webrtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override{
    }

    void OnRenegotiationNeeded() override{
    }

    void OnNegotiationNeededEvent(uint32_t) override{
    }

    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState) override{
    }

    void OnStandardizedIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState) override{
    }

    void OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState) override{
    }

    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override{
    }

    void OnIceCandidate(const webrtc::IceCandidate *candidate) override{
    }

    void OnIceCandidateError(const std::string &, int, const std::string &, int, const std::string &) override{
    }

    void OnIceCandidatesRemoved(const std::vector<webrtc::Candidate> &) override{
    }

    void OnIceConnectionReceivingChange(bool) override{
    }

    void OnIceSelectedCandidatePairChanged(const webrtc::CandidatePairChangeEvent &) override{
    }

    void OnAddTrack(webrtc::scoped_refptr<webrtc::RtpReceiverInterface>,
        const std::vector<webrtc::scoped_refptr<webrtc::MediaStreamInterface>> &) override{
    }

    void OnTrack(webrtc::scoped_refptr<webrtc::RtpTransceiverInterface>) override{
    }

    void OnRemoveTrack(webrtc::scoped_refptr<webrtc::RtpReceiverInterface>) override{
    }

    void OnInterestingUsage(int) override{
    }

    void call(){
        auto signaling_thread = webrtc::Thread::CreateWithSocketServer();
        signaling_thread->Start();
        auto worker_thread = webrtc::Thread::Create();
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

        if(factory){
            webrtc::PeerConnectionInterface::RTCConfiguration configuration;
            webrtc::PeerConnectionDependencies dependencies(this);

            auto result = factory->CreatePeerConnectionOrError(configuration, std::move(dependencies));

            if (!result.ok()) {
                std::cerr << "Failed to create PeerConnection or error" << std::endl;
                return;
            }

            this->peer_connection_= result.value();

            webrtc::scoped_refptr<AudioSource> audio_track_source = webrtc::make_ref_counted<AudioSource>();
            webrtc::scoped_refptr<VideoTrackSource> video_track_source = webrtc::make_ref_counted<VideoTrackSource>();

            webrtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(factory->CreateAudioTrack("audio", audio_track_source.get()));

            auto r = this->peer_connection_->AddTrack(audio_track, {"stream-id"});

            if (!r.ok()) {
                std::cerr << "Failed to add Audio Track" << std::endl;
            }

            this->peer_connection_->CreateOffer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());

          }else {
              std::cerr << "Failed to create PeerConnectionFactory" << std::endl;
          }
      }

      webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> createFactory(){
        auto signaling_thread = webrtc::Thread::CreateWithSocketServer();
        signaling_thread->Start();
        auto worker_thread = webrtc::Thread::Create();
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
private:
    webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
    SessionDescriptionObserver sdp_observer_;
};



#endif //PEER_CONNECTION_H
