//
// Created by bimba on 7/24/25.
//

#include "audio_capture_module.hpp"


AudioCaptureModule::AudioCaptureModule(): data_{NULL}
{
}

AudioCaptureModule::~AudioCaptureModule()
{
    webrtc::MutexLock lock(&this->mutex_);

    if (this->processing_thread_)
    {
        this->processing_thread_->Stop();
    }
}

inline int32_t AudioCaptureModule::ActiveAudioLayer(AudioLayer* audioLayer) const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::RegisterAudioCallback(webrtc::AudioTransport* audioCallback)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;

    bool created = false;

    {
        webrtc::MutexLock lock(&this->mutex_);
        this->audio_callback_ = audioCallback;

        if (!this->processing_thread_)
        {
            this->processing_thread_ = rtc::Thread::Create();
            this->processing_thread_->Start();
            created = true;
        }
    }

    if (created)
    {
        this->processing_thread_->PostTask([this]
        {
           this->process_frame();
        });
    }
    return 0;
}

void AudioCaptureModule::process_frame()
{
    int64_t time1 = rtc::TimeMillis();

    {
        webrtc::MutexLock lock(&this->mutex_);
        size_t n_samples_out = 0;
        int64_t elapsed_time_ms = 0;
        int64_t ntp_time_ms = 0;
        memset(this->data_, 0, sizeof(this->data_));
        if (this->audio_callback_)
        {
            this->audio_callback_->NeedMorePlayData(AUDIO_SAMPLES_IN_WEBRTC_FRAME, 2,1, AUDIO_SAMPLE_RATE, this->data_, n_samples_out, &elapsed_time_ms, &ntp_time_ms);
        }
    }

    int64_t time2 = rtc::TimeMillis();
    int64_t next_frame_time = time1 + this->frame_length_;
    int64_t wait_time = next_frame_time - time2;

    this->processing_thread_->PostDelayedTask([this]()->void
    {
        this->process_frame();
    }, webrtc::TimeDelta::Millis(wait_time));
}

inline int32_t AudioCaptureModule::Init()
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::Terminate()
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline bool AudioCaptureModule::Initialized() const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return true;
}

inline int16_t AudioCaptureModule::PlayoutDevices()
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int16_t AudioCaptureModule::RecordingDevices()
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::PlayoutDeviceName(uint16_t index, char name[128], char guid[128])
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::RecordingDeviceName(uint16_t index, char name[128], char guid[128])
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::SetPlayoutDevice(uint16_t index)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::SetPlayoutDevice(WindowsDeviceType device)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::SetRecordingDevice(uint16_t index)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::SetRecordingDevice(WindowsDeviceType device)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::PlayoutIsAvailable(bool* available)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::InitPlayout()
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline bool AudioCaptureModule::PlayoutIsInitialized() const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return true;
}

inline int32_t AudioCaptureModule::RecordingIsAvailable(bool* available)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::InitRecording()
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline bool AudioCaptureModule::RecordingIsInitialized() const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return false;
}

inline int32_t AudioCaptureModule::StartPlayout()
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::StopPlayout()
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline bool AudioCaptureModule::Playing() const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return true;
}

inline int32_t AudioCaptureModule::StartRecording()
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::StopRecording()
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline bool AudioCaptureModule::Recording() const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return false;
}

inline int32_t AudioCaptureModule::InitSpeaker()
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline bool AudioCaptureModule::SpeakerIsInitialized() const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return true;
}

inline int32_t AudioCaptureModule::InitMicrophone()
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline bool AudioCaptureModule::MicrophoneIsInitialized() const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return true;
}

inline int32_t AudioCaptureModule::SpeakerVolumeIsAvailable(bool* available)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::SetSpeakerVolume(uint32_t volume)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::SpeakerVolume(uint32_t* volume) const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::MaxSpeakerVolume(uint32_t* maxVolume) const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::MinSpeakerVolume(uint32_t* minVolume) const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::MicrophoneVolumeIsAvailable(bool* available)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::SetMicrophoneVolume(uint32_t volume)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::MicrophoneVolume(uint32_t* volume) const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::MaxMicrophoneVolume(uint32_t* maxVolume) const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::MinMicrophoneVolume(uint32_t* minVolume) const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::SpeakerMuteIsAvailable(bool* available)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::SetSpeakerMute(bool enable)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::SpeakerMute(bool* enabled) const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::MicrophoneMuteIsAvailable(bool* available)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::SetMicrophoneMute(bool enable)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::MicrophoneMute(bool* enabled) const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::StereoPlayoutIsAvailable(bool* available) const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::SetStereoPlayout(bool enable)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::StereoPlayout(bool* enabled) const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::StereoRecordingIsAvailable(bool* available) const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::SetStereoRecording(bool enable)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::StereoRecording(bool* enabled) const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return 0;
}

inline int32_t AudioCaptureModule::PlayoutDelay(uint16_t* delayMS) const
{
    // std::cout << "AudioCaptureModule " << __func__ << std::endl;
    *delayMS = 0;
    return 0;
}

inline bool AudioCaptureModule::BuiltInAECIsAvailable() const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return false;
}

inline bool AudioCaptureModule::BuiltInAGCIsAvailable() const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return false;
}

inline bool AudioCaptureModule::BuiltInNSIsAvailable() const
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return false;
}

inline int32_t AudioCaptureModule::EnableBuiltInAEC(bool enable)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return -1;
}

inline int32_t AudioCaptureModule::EnableBuiltInAGC(bool enable)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return -1;
}

inline int32_t AudioCaptureModule::EnableBuiltInNS(bool enable)
{
    std::cout << "AudioCaptureModule " << __func__ << std::endl;
    return -1;
}
