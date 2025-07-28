//
// Created by bimba on 7/26/25.
//

#include "audio_device_info.hpp"

AudioDeviceInfo::AudioDeviceInfo(const char* p_name, int index):
AudioDeviceInfo(std::string{p_name}, index)
{
}

AudioDeviceInfo::AudioDeviceInfo(std::string name, int index):
name_(name), index_(index)
{
}

const std::string& AudioDeviceInfo::getName() const
{
    return this->name_;
}

int AudioDeviceInfo::getIndex() const
{
    return this->index_;
}

std::string AudioDeviceInfo::str() const
{
    return "[AudioDeviceInfo] Name: " + name_ + ", Index: " + std::to_string(index_);
}

