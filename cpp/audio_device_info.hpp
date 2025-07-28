//
// Created by bimba on 7/26/25.
//

#ifndef AUDIO_DEVICE_INFO_H
#define AUDIO_DEVICE_INFO_H

#include <string>

#include "portaudio.h"

class AudioDeviceInfo {
public:
    AudioDeviceInfo(const char *p_name, int index);
    AudioDeviceInfo(std::string name, int index);
    ~AudioDeviceInfo() = default;
    const std::string &getName() const;
    int getIndex() const;
    std::string str() const;

private:
    std::string name_;
    int index_;
};



#endif //AUDIO_DEVICE_INFO_H
