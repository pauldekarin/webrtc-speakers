//
// Created by bimba on 8/5/25.
//

#ifndef VIRTUAL_AUDIO_OUTPUT_H
#define VIRTUAL_AUDIO_OUTPUT_H

#include <set>
#include <pulse/pulseaudio.h>
#include "settings.hpp"

class VirtualAudioOutput
{
public:
    VirtualAudioOutput();
    ~VirtualAudioOutput();

    const std::shared_ptr<spdlog::logger>& get_logger() const;
    bool module_loaded() const;

private:
    static void context_state_sb_(pa_context* ctx, void* user_data);
    static void module_load_cb_(pa_context* ctx, uint32_t idx, void* user_data);
    static std::string get_state_as_string_(pa_context_state_t state);
    static std::shared_ptr<spdlog::logger> root_logger_;

    void on_changed_state_();
    void on_module_loaded_(uint32_t index);
    void setup_logger_();

    uint32_t index_;
    pa_context* context_;
    pa_mainloop* mainloop_;

    std::shared_ptr<spdlog::logger> logger_;
};


#endif //VIRTUAL_AUDIO_OUTPUT_H
