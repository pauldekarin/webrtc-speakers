//
// Created by bimba on 7/26/25.
//

#ifndef AUDIO_DEVICE_INFO_H
#define AUDIO_DEVICE_INFO_H

#include <string>

#include "portaudio.h"

class AudioDeviceInfo
{
public:
    AudioDeviceInfo(const char* p_name, int index, int output_channels, int input_channels);
    AudioDeviceInfo(std::string name, int index, int output_channels, int input_channels);
    ~AudioDeviceInfo() = default;
    const std::string& get_name() const;
    int get_index() const;
    int get_output_channels() const;
    int get_input_channels() const;
    std::string str() const;

private:
    std::string name_;
    int index_;
    int output_channels_;
    int input_channels_;
};


#endif //AUDIO_DEVICE_INFO_H
