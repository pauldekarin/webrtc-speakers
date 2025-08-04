//
// Created by bimba on 8/1/25.
//

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include "spdlog/sinks/stdout_color_sinks.h"

using log_sink = spdlog::sinks::stdout_color_sink_mt;

class Settings
{
public:
    static void load(int argc, const char** argv);
    static std::string get_signaling_server_url();

private:
    static std::string signaling_server_url_;
};

#endif //SETTINGS_H
