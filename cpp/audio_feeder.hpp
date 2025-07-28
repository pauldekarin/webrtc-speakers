//
// Created by bimba on 7/26/25.
//

#ifndef AUDIOFEEDER_H
#define AUDIOFEEDER_H
#include <atomic>
#include <vector>

#include "audio_config.hpp"
#include "audio_device_info.hpp"
#include "audio_handler_interface.hpp"
#include "portaudio.h"

typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
typedef int16_t Sample;
class AudioFeeder;


typedef struct
{
    int          frameIndex;  /* Index into sample array. */
    int          maxFrameIndex;
    SAMPLE      *recordedSamples;
}
paTestData;

typedef struct
{
    AudioFeeder* feeder;
    std::vector<Sample>* data;
}paData;

class AudioFeeder {
public:
    AudioFeeder(AudioConfig config);
    AudioFeeder(AudioConfig config, AudioHandlerInterface* sink);

    ~AudioFeeder() = default;

    void start(AudioDeviceInfo info);
    void stop();
    bool is_available();
    bool is_feeding();
    void set_sink(AudioHandlerInterface* sink);
    void on_data(const void *buffer, unsigned long frames_per_buffer);

    const AudioConfig get_audio_config() const;

    std::vector<AudioDeviceInfo> get_audio_devices();
    AudioDeviceInfo get_default_output_device();

    AudioHandlerInterface *sink_;
    AudioConfig config_;
    PaStream *stream_;
    paData *data_;

    static bool library_loaded;

    static int callback_(const void *input_buffer, void *output_buffer,
                         unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo *time_info,
                         PaStreamCallbackFlags status_flags, void *user_data);
    static int record_(const void *input_buffer, void *output_buffer,
                         unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo *time_info,
                         PaStreamCallbackFlags status_flags, void *user_data);
    static bool load_library_();
    static AudioDeviceInfo map_info_(const PaDeviceInfo *info);
    static const PaDeviceInfo *map_info_(AudioDeviceInfo info);
};


#endif //AUDIOFEEDER_H
