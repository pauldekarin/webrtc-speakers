//
// Created by bimba on 7/26/25.
//

#ifndef AUDIO_CONFIG_H
#define AUDIO_CONFIG_H



class AudioConfig {
public:
    AudioConfig(int sample_rate, int channels, int frames_per_buffer, unsigned long format);

    int getSampleRate() const;
    int getChannels() const;
    int getFramesPerBuffer() const;
    unsigned long getFormat() const;

private:
    int sample_rate_;
    int channels_;
    int frames_per_buffer_;
    unsigned long format_;
};



#endif //AUDIO_CONFIG_H
