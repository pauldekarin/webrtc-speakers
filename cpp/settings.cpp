//
// Created by bimba on 8/1/25.
//

#include "settings.hpp"

std::string Settings::signaling_server_url_ = "";

void Settings::load(int argc, const char **argv)
{
    signaling_server_url_ = std::string(argv[1]);
}

std::string Settings::get_signaling_server_url()
{
    return signaling_server_url_;
}



