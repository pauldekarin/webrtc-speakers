//
// Created by bimba on 8/5/25.
//

#include "virtual_audio_output.hpp"

#include "spdlog/spdlog.h"

std::shared_ptr<spdlog::logger> VirtualAudioOutput::root_logger_ = spdlog::create<
    log_sink>("VirtualAudioOutput-static");

VirtualAudioOutput::VirtualAudioOutput():
    index_(PA_INVALID_INDEX)
{
    this->setup_logger_();

    this->mainloop_ = pa_mainloop_new();

    if (!this->mainloop_)
    {
        this->logger_->error("Failed to create main loop");

        throw std::runtime_error("Failed to create main loop");
    }

    pa_mainloop_api* api = pa_mainloop_get_api(this->mainloop_);
    this->context_ = pa_context_new(api, "virtual_audio_output");

    if (!this->context_)
    {
        this->logger_->error("Failed to create context");
        throw std::runtime_error("Failed to create context");
    }

    pa_context_set_state_callback(this->context_, &VirtualAudioOutput::context_state_sb_, this);

    if (pa_context_connect(this->context_, nullptr, PA_CONTEXT_NOFLAGS, nullptr) < 0)
    {
        this->logger_->error("Failed to connect context. Error: {}", pa_strerror(pa_context_errno(this->context_)));
        throw std::runtime_error("Failed to connect context");
    }

    while (!module_loaded())
    {
        this->logger_->info("Iteration");

        int ret = pa_mainloop_iterate(this->mainloop_, 1, nullptr);

        if (ret < 0)
        {
            this->logger_->error("Failed to main loop. Error: {}", pa_strerror(ret));

            throw std::runtime_error("Failed to main loop");
        }
    }
}

VirtualAudioOutput::~VirtualAudioOutput()
{
    if (this->context_)
    {
        if (this->module_loaded())
        {
            pa_context_unload_module(this->context_, this->index_, nullptr, nullptr);
        }

        if (pa_context_get_state(this->context_) == PA_CONTEXT_READY)
        {
            pa_context_disconnect(this->context_);
        }
        pa_context_unref(this->context_);
    }
    if (this->mainloop_)
    {
        pa_mainloop_free(this->mainloop_);
    }
}

const std::shared_ptr<spdlog::logger>& VirtualAudioOutput::get_logger() const
{
    return this->logger_;
}

bool VirtualAudioOutput::module_loaded() const
{
    return this->index_ != PA_INVALID_INDEX;
}

void VirtualAudioOutput::on_changed_state_()
{
    pa_context_state_t state = pa_context_get_state(this->context_);

    this->logger_->info("Change state to: {}", get_state_as_string_(state));

    if (state == PA_CONTEXT_READY)
    {
        std::string module_args = "sink_name=VirtualSink sink_properties=device.description=Virtual_Output";
        pa_operation* operation = pa_context_load_module(this->context_, "module-null-sink", module_args.c_str(),
                                                         &VirtualAudioOutput::module_load_cb_, this);

        if (!operation)
        {
            this->logger_->error("Failed to load module. Error: {}", pa_strerror(pa_context_errno(this->context_)));

            throw std::runtime_error("Failed to load module");
        }
        else
        {
            pa_operation_unref(operation);
        }
    }
    else
    {
    }
}

void VirtualAudioOutput::on_module_loaded_(uint32_t index)
{
    if (index != PA_INVALID_INDEX)
    {
        this->logger_->info("Module successfully loaded. Index: {}", index);

        this->index_ = index;
    }
    else
    {
        this->logger_->error("Invalid index");

        throw std::runtime_error("Invalid index");
    }
}

void VirtualAudioOutput::setup_logger_()
{
    this->logger_ = spdlog::create<log_sink>("VirtualAudioOutput");
}

void VirtualAudioOutput::module_load_cb_(pa_context* ctx, uint32_t idx, void* user_data)
{
    if (!user_data)
    {
        root_logger_->error("Failed to get user data. Skip module loading event");

        return;
    }

    static_cast<VirtualAudioOutput*>(user_data)->on_module_loaded_(idx);
}

std::string VirtualAudioOutput::get_state_as_string_(pa_context_state_t state)
{
    switch (state)
    {
    case PA_CONTEXT_READY:
        return "PA_CONTEXT_READY";
    case PA_CONTEXT_FAILED:
        return "PA_CONTEXT_FAILED";
    case PA_CONTEXT_TERMINATED:
        return "PA_CONTEXT_TERMINATED";
    case PA_CONTEXT_UNCONNECTED:
        return "PA_CONTEXT_UNCONNECTED";
    case PA_CONTEXT_AUTHORIZING:
        return "PA_CONTEXT_AUTHORIZING";
    case PA_CONTEXT_SETTING_NAME:
        return "PA_CONTEXT_SETTING_NAME";
    case PA_CONTEXT_CONNECTING:
        return "PA_CONTEXT_CONNECTING";
    default:
        return "Unknown";
    }
}

void VirtualAudioOutput::context_state_sb_(pa_context* ctx, void* user_data)
{
    if (!user_data)
    {
        root_logger_->error("Failed to get user data");

        return;
    }

    static_cast<VirtualAudioOutput*>(user_data)->on_changed_state_();
}
