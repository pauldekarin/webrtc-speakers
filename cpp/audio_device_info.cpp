//
// Created by bimba on 7/26/25.
//

#include "audio_device_info.hpp"

AudioDeviceInfo::AudioDeviceInfo(const char* p_name, int index, int input_channels, int output_channels):
    AudioDeviceInfo(std::string{p_name}, index, input_channels, output_channels)
{
}

AudioDeviceInfo::AudioDeviceInfo(std::string name, int index, int input_channels, int output_channels):
    name_(name), index_(index), input_channels_(input_channels), output_channels_(output_channels)
{
}

const std::string& AudioDeviceInfo::get_name() const
{
    return this->name_;
}

int AudioDeviceInfo::get_index() const
{
    return this->index_;
}

int AudioDeviceInfo::get_output_channels() const
{
    return this->output_channels_;
}

int AudioDeviceInfo::get_input_channels() const
{
    return this->input_channels_;
}

std::string AudioDeviceInfo::str() const
{
    return "Name: " + name_ + ", Index: " + std::to_string(index_) + ", In/Out channels: " +
        std::to_string(input_channels_) + "/" + std::to_string(output_channels_);
}

