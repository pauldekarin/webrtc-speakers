//
// Created by bimba on 7/26/25.
//

#include "audio_config.hpp"

AudioConfig::AudioConfig(int sample_rate, int channels, int frames_per_buffer, unsigned long format):
sample_rate_(sample_rate), channels_(channels), frames_per_buffer_(frames_per_buffer), format_(format)
{

}

int AudioConfig::getSampleRate() const
{
    return this->sample_rate_;
}

int AudioConfig::getChannels() const
{
    return this->channels_;
}

int AudioConfig::getFramesPerBuffer() const
{
    return this->frames_per_buffer_;
}


unsigned long AudioConfig::getFormat() const
{
    return this->format_;
}

