#include <utility>
#include <fstream>
#include <iostream>
#include "portaudio.h"
#include "pulse/simple.h"
#include "pulse/error.h"

#include <webrtc/modules/audio_coding/codecs/audio_decoder.h>

#define SAMPLE_RATE (44100)
#define NUM_CHANNELS (2)
#define NUM_SECONDS (3)
#define PA_SAMPLE_TYPE paFloat32
#define FRAMES_PER_BUFFER (512)
#define SAMPLE_SILENCE (0)

typedef float SAMPLE;

typedef struct{
    int frame_index;
    int max_frame_index;
    SAMPLE *recorded_samples;
} paTestData;

static int record_callback(const void *input_buffer, void *output_buffer,
    unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo *time_info,
        PaStreamCallbackFlags status_flags, void *user_data
){
    paTestData *data = (paTestData*) user_data;
    const SAMPLE *rptr = (const SAMPLE*)input_buffer;
    SAMPLE *wptr = &data->recorded_samples[data->frame_index * NUM_CHANNELS];
    long frames_to_calc;
    long i;
    int finished;
    unsigned int frames_left = data->max_frame_index - data->frame_index;

    if (frames_left < frames_per_buffer){
        frames_to_calc = frames_left;
        finished = paComplete;
    }else{
        frames_to_calc = frames_per_buffer;
        finished = paContinue;
    }

    if(input_buffer == nullptr){
        for(i = 0; i < frames_to_calc; i++){
            *wptr++ = SAMPLE_SILENCE;
            if(NUM_CHANNELS == 2) *wptr++ = SAMPLE_SILENCE;
        }
    }else{
        for( i = 0; i < frames_to_calc; i++){
            *wptr++ = *rptr++;
            if(NUM_CHANNELS == 2){
                *wptr++ = *rptr++;
            }
        }
    }

    data->frame_index += frames_to_calc;

    return finished;
}

static int play_callback(const void *input_buffer, void *output_buffer,
    unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo *time_info,
    PaStreamCallbackFlags status_flags, void *user_data
){
    // paTestData *data = (paTestData*)user_data;
    // SAMPLE *rptr = &data->recorded_samples[data->frame_index * NUM_CHANNELS];
    // SAMPLE *wptr = (SAMPLE*)output_buffer;
    // unsigned int i;
    // int finished;
    // unsigned int frames_left = data->max_frame_index - data->frame_index;

    // if(frames_left < frames_per_buffer){
    //     for(i = 0; i < frames_left; i++){
    //         *wptr++ = *rptr++;
    //         if(NUM_CHANNELS == 2) *wptr++ = *rptr++;
    //     }
    //     data->frame_index += frames_left;
    //     finished = paComplete;
    // }else{
    //     for(i = 0 ; i < frames_per_buffer; i++){
    //         *wptr++ = *rptr++;
    //         if(NUM_CHANNELS == 2) *wptr++ = *rptr++;
    //     }
    //     data->frame_index += frames_per_buffer;
    //     finished = paContinue;
    // }
    // return finished;
    return paComplete;
}


static void handle_error(PaError pa_error){
    if(pa_error != paNoError){
        std::cout << "Occured some error\n";
        Pa_Terminate();

        std::exit(pa_error);
    }
}

static void get_audio_devices(){
    std::cout << "== get audio devices info ==" << std::endl;

    int num_devices = Pa_GetDeviceCount();

    if(num_devices <= 0){
        std::cout << "count <= 0" << std::endl;
        return;
    }

    for(int i = 0 ; i < num_devices; i++){
        const PaDeviceInfo *device_info = Pa_GetDeviceInfo(i);

        std::cout << device_info->name << " -- " << i << std::endl;
    }
}

int main(int argc, char **argv){
    // pa_sample_spec spec = {
    //     .format = PA_SAMPLE_S16LE,
    //     .rate = SAMPLE_RATE,
    //     .channels = NUM_CHANNELS
    // };

    // pa_simple *stream = nullptr;
    // int ret = 0;
    // int error;

    // if(!(stream = pa_simple_new(
    //     NULL,
    //     argv[0],
    //     PA_STREAM_PLAYBACK,
    //     nullptr,
    //     "playback",
    //     &spec,
    //     nullptr,
    //     nullptr,
    //     &error
    // ))){
    //     std::cerr << "pa_simple_new occured some errors!\n";
    //     std::exit(-1);
    // }

    // pa_usec_t latency;


    // if((latency = pa_simple_get_latency(stream, &error)) == (pa_usec_t) - 1){
    //     std::cerr << "pa_simple_get_latency occurred some errors!\n";
    //     std::exit(-1);
    // }

    // for(int i = 0; SAMPLE_RATE * 10 / FRAMES_PER_BUFFER; i++){
    //     std::uint8_t buffer[FRAMES_PER_BUFFER];

    //     for(int j = 0; j < FRAMES_PER_BUFFER; j++){
    //         buffer[j] = 255;
    //     }

    //     if(pa_simple_write(stream, buffer, sizeof(buffer), &error) < 0){
    //         std::cerr << "pa_simple_write() failed: " << pa_strerror(error) << std::endl;
    //         std::exit(error);
    //     }
    // }


    // if(pa_simple_drain(stream, &error) < 0){
    //     std::cerr << "pa_simple_Drain() failed: " << pa_strerror(error) << std::endl;
    //     std::exit(error);
    // }

    // if(stream != nullptr){
    //     pa_simple_free(stream);
    // }
    PaStreamParameters input_params;
    PaStream *stream;
    paTestData data;

    int total_frames;
    int num_samples;
    int num_bytes;

    data.max_frame_index = total_frames = NUM_SECONDS * SAMPLE_RATE;
    data.frame_index = 0;
    num_samples = total_frames * NUM_CHANNELS;
    num_bytes = sizeof(SAMPLE) * num_samples;
    std::cout << "Bytes: " << num_bytes << std::endl;
    data.recorded_samples = new SAMPLE[num_bytes];

    if(data.recorded_samples == nullptr){
        std::cerr << "Cannot allocate" << std::endl;
        return 0;
    }

    handle_error(Pa_Initialize());

    get_audio_devices();
    input_params.device = Pa_GetDefaultInputDevice();
    if(input_params.device == paNoDevice){
        std::cout << "There is no default input device" << std::endl;
        return paNoDevice;
    }

    // input_params.channelCount = NUM_CHANNELS;
    // input_params.sampleFormat = PA_SAMPLE_TYPE;
    // input_params.suggestedLatency = Pa_GetDeviceInfo(input_params.device)->defaultLowInputLatency;
    // input_params.hostApiSpecificStreamInfo = nullptr;

    // handle_error(Pa_OpenStream(&stream, &input_params, nullptr, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff, record_callback, &data));

    // handle_error(Pa_StartStream(stream));

    // while((Pa_IsStreamActive(stream)) == 1){
    //     Pa_Sleep(1000);
    //     std::cout << "Recorded: " << data.frame_index << std::endl;
    // }

    // std::ofstream output_file("output.raw");

    // if(output_file.is_open()){
    //     output_file << data.recorded_samples;
    // }else{
    //     std::cerr << "can't open file" << std::endl;
    // }

    // handle_error(Pa_CloseStream(stream));

    PaStreamParameters output_parameters;
    output_parameters.device = Pa_GetDefaultOutputDevice();

    if(output_parameters.device == paNoDevice){
        std::cerr << "There is no any default output device";
        std::exit(paNoDevice);
    }
    output_parameters.channelCount = NUM_CHANNELS;
    output_parameters.sampleFormat = PA_SAMPLE_TYPE;
    output_parameters.suggestedLatency = Pa_GetDeviceInfo(output_parameters.device)->defaultLowOutputLatency;
    output_parameters.hostApiSpecificStreamInfo = nullptr;

    data.frame_index = 0;

    handle_error(Pa_OpenStream(&stream, nullptr, &output_parameters, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff, play_callback, &data));

    handle_error(Pa_StartStream(stream));
    int i = 0;

    while((Pa_IsStreamActive(stream)) == 1){
        if(i > 7){
            break;
        }
        i++;
        std::cout << "AA" << std::endl;
        Pa_Sleep(1000);
    }

    if(data.recorded_samples != nullptr){
        delete[] data.recorded_samples;
    }

    handle_error(Pa_CloseStream(stream));


    handle_error(Pa_Terminate());
}