//
// Created by bimba on 7/26/25.
//

#ifndef AUDIOHANDLERINTERFACE_H
#define AUDIOHANDLERINTERFACE_H

class AudioHandlerInterface
{
public:
    AudioHandlerInterface() = default;
    virtual ~AudioHandlerInterface() = default;

    virtual void handle(const void* audio_data, int bits_per_sample, int sample_rate, size_t number_of_channels,
    size_t number_of_frames) = 0;
};
#endif //AUDIOHANDLERINTERFACE_H
