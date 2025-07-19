//
// Created by bimba on 27.06.25.
//

#ifndef SIGNALINGCLIENT_H
#define SIGNALINGCLIENT_H

#include <iostream>
#include <queue>
#include <thread>
#include <string>
#include <memory>
#include <atomic>
#include <functional>
#include "libwebsockets.h"
#include <atomic>
#include <sstream>
#include <condition_variable>
#include <mutex>

#include "signaling_client_interface.hpp"

class Conductor;

class Messages
{
public:
    Messages();

    void send(const std::string& message);
    void close();
    void set_wsi(struct lws *wsi);

private:
    void send_(const std::string& message) const;
    void loop_();

    struct lws *wsi_;
    std::atomic<bool> running_;

    std::mutex queue_mutex_;
    std::queue<std::string> queue_;

    std::condition_variable loop_cv_;
    std::mutex loop_mutex_;
    std::thread loop_thread_;
};

class SignalingClient: public SignalingClientInterface{
public:
    SignalingClient(Conductor *conductor);
    ~SignalingClient() override;

    void connect(const std::string &address, const std::string &path, int port) override;
    void send(const std::string &message)  override;
    void receive(const char *p_message, size_t len)  override;
    void disconnect() override;
    void destroy();
    void join();

private:
    static int callback_(struct lws *wsi, enum lws_callback_reasons reason, void *userdata, void *in, size_t len);
    void loop_();

    std::unique_ptr<struct lws_context, void(*)(struct lws_context*)> ctx_;
    std::unique_ptr<Messages> messages_;
    std::stringstream in_stream_;

    std::atomic<bool> running_{false};
    std::thread service_thread_;
    const std::string name_ = "SignalingClient";
    struct lws *wsi_ = nullptr;

    Conductor *conductor_;


};



#endif //SIGNALINGCLIENT_H
