//
// Created by bimba on 27.06.25.
//

#ifndef SIGNALINGCLIENT_H
#define SIGNALINGCLIENT_H

#include <iostream>
#include <thread>
#include <string>
#include <memory>
#include <atomic>
#include <functional>
#include "libwebsockets.h"

class SignalingClient {
public:
    static void builtin_log(int argc, char **argv);

    SignalingClient();
    ~SignalingClient();

    void connect(const std::string &address, const std::string &path, int port);
    void send(const char *p_str);
    void disconnect();
    void destroy();
    void join();

private:
    static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *userdata, void *in, size_t len);

    void loop_();

    std::atomic<bool> running_;
    std::thread service_thread_;
    std::unique_ptr<struct lws_context, void(*)(struct lws_context*)> ctx_;
    std::unique_ptr<struct lws> lws_;
    const std::string name_ = "SignalingClient";
};



#endif //SIGNALINGCLIENT_H
