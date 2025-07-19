//
// Created by bimba on 7/19/25.
//

#ifndef SIGNALING_CLIENT_INTERFACE_H
#define SIGNALING_CLIENT_INTERFACE_H
#include <string>

class SignalingClientInterface
{
public:
    virtual ~SignalingClientInterface() = default;
    virtual void connect(const std::string &address, const std::string &path, int port);
    virtual void disconnect();
    virtual void send(const std::string &message);
    virtual void receive(const char *p_message, size_t len);
};

#endif //SIGNALING_CLIENT_INTERFACE_H
