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
#include <regex>

#include "settings.hpp"
#include "signaling_client_interface.hpp"

class Conductor;

class Messages
{
public:
    Messages();

    void send(const std::string& message);
    void close();
    void set_wsi(struct lws* wsi);

private:
    void send_(const std::string& message) const;
    void loop_();

    struct lws* wsi_;
    std::atomic<bool> running_;

    std::mutex queue_mutex_;
    std::queue<std::string> queue_;

    std::condition_variable loop_cv_;
    std::mutex loop_mutex_;
    std::thread loop_thread_;
};

typedef struct
{
    std::string protocol;
    std::string host;
    std::string path;
    int port;
} address;

class SignalingClient : public SignalingClientInterface
{
public:
    SignalingClient();
    explicit SignalingClient(Conductor* conductor = nullptr);
    ~SignalingClient() override;

    void connect(const std::string& url) override;
    void send(const std::string& message) override;
    void receive(const char* p_message, long len) override;
    void disconnect() override;
    void destroy();
    void join();
    std::string describe() override;

private:
    static int callback_(struct lws* wsi, enum lws_callback_reasons reason, void* userdata, void* in, size_t len);
    static address parse_url_(const std::string& url);

    void setup_context_();
    void setup_logger_(std::string logger_name);
    void loop_();

    std::unique_ptr<struct lws_context, void(*)(struct lws_context*)> ctx_;
    std::unique_ptr<Messages> messages_;
    std::stringstream in_stream_;

    std::atomic<bool> running_{false};
    std::thread service_thread_;
    struct lws* wsi_ = nullptr;

    Conductor* conductor_;

    std::shared_ptr<spdlog::logger> logger_;
};


#endif //SIGNALINGCLIENT_H
