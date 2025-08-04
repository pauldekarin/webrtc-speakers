//
// Created by bimba on 27.06.25.
//

#include "signaling_client.hpp"

#include <utility>

#include "conductor.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

Messages::Messages():
    running_(true), wsi_(nullptr)
{
    this->loop_thread_ = std::thread(&Messages::loop_, this);
}

void Messages::send(const std::string& message)
{
    lwsl_user("[%s] Pull message: %s", __func__, message.c_str());
    std::lock_guard<std::mutex> lock(this->queue_mutex_);
    this->queue_.push(message);
    this->loop_cv_.notify_all();
}

void Messages::set_wsi(struct lws* wsi)
{
    if (wsi == nullptr)
    {
        return;
    }

    this->wsi_ = wsi;
    this->loop_cv_.notify_all();
}

void Messages::close()
{
    this->running_.store(false);
}


void Messages::loop_()
{
    while (this->running_.load())
    {
        std::unique_lock<std::mutex> lock(this->loop_mutex_);

        lwsl_user("Park");
        this->loop_cv_.wait(lock, [&]
        {
            return !this->running_.load() || (this->wsi_ != nullptr && !this->queue_.empty());
        });

        std::lock_guard<std::mutex> lock_messages(this->queue_mutex_);
        while (!this->queue_.empty())
        {
            const std::string& msg = this->queue_.front();

            this->send_(msg);

            this->queue_.pop();
        }
    }
}


void Messages::send_(const std::string& message) const
{
    if (this->wsi_ == nullptr)
    {
        lwsl_user("Cannot send: locked or invalid wsi");
        return;
    }

    lwsl_user("Send message: %s", message.c_str());

    unsigned char buf[LWS_PRE + message.size() + 1];
    memset(buf, 0, LWS_PRE + message.size() + 1);
    memcpy(&buf[LWS_PRE], message.c_str(), message.size());

    if (lws_write(this->wsi_, buf + LWS_PRE, message.size(), LWS_WRITE_TEXT) < 0)
    {
        lwsl_err("Failed to write to wsi");
        return;
    }

    lws_callback_on_writable(this->wsi_);
}


SignalingClient::SignalingClient(): SignalingClient(nullptr)
{
}

SignalingClient::SignalingClient(Conductor* conductor):
    ctx_(nullptr, [](struct lws_context* ctx) { if (ctx != nullptr) lws_context_destroy(ctx); }),
    messages_(std::make_unique<Messages>()),
    conductor_(conductor)
{
    this->setup_logger_("SignalingClient");
    this->setup_context_();
}

SignalingClient::~SignalingClient()
{
    this->SignalingClient::disconnect();
}


void SignalingClient::send(const std::string& message)
{
    this->messages_->send(message);
}

void SignalingClient::receive(const char* p_message, const long len)
{
    if (p_message == nullptr || len == 0)
    {
        return;
    }

    this->in_stream_.write(p_message, len);

    if (lws_is_final_fragment(this->wsi_))
    {
        std::string message = this->in_stream_.str();
        this->in_stream_.str("");
        this->in_stream_.clear();

        this->logger_->info("Received message: {}", message);

        if (this->conductor_)
        {
            this->conductor_->handle_message(message);
        }
    }
}

void SignalingClient::connect(const std::string& url)
{
    this->setup_logger_("SignalingClient-" + url);

    this->logger_->info("Trying to connect {}", url);

    const address addr = parse_url_(url);

    struct lws_client_connect_info info = {};
    info.address = addr.host.c_str();
    info.port = addr.port;
    info.path = addr.path.c_str();
    info.host = addr.host.c_str();
    info.context = this->ctx_.get();
    info.ssl_connection = LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED | LCCSCF_ALLOW_INSECURE |
        LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;

    struct lws* conn = lws_client_connect_via_info(&info);

    if (conn == nullptr)
    {
        this->logger_->error("Failed to connect {}", url);
        return;
    }


    this->wsi_ = conn;
    this->messages_->set_wsi(this->wsi_);
    this->service_thread_ = std::thread(&SignalingClient::loop_, this);

    this->logger_->info("Connected to {}", url);
}


void SignalingClient::disconnect()
{
    if (this->service_thread_.joinable())
    {
        this->service_thread_.join();
    }
    this->running_.store(false);
    this->destroy();
}

void SignalingClient::destroy()
{
    if (this->ctx_)
    {
        lws_context_destroy(this->ctx_.get());
        this->ctx_.reset(nullptr);
    }
}

void SignalingClient::join()
{
    if (this->service_thread_.joinable())
    {
        this->service_thread_.join();
    }
}

std::string SignalingClient::describe()
{
    return this->logger_->name();
}

void SignalingClient::loop_()
{
    this->running_.store(true);
    while (this->running_.load() && this->ctx_)
    {
        int ret = lws_service(this->ctx_.get(), 1000);
        if (ret < 0)
        {
            this->logger_->error("Failed to service {}", ret);
            break;
        }
    }
}

address SignalingClient::parse_url_(const std::string& url)
{
    address result;

    std::regex url_regex(R"(^(\w+):\/\/([^\/:]+)(?::(\d+))?(/.*)?$)");
    std::smatch match;

    if (!std::regex_match(url, match, url_regex))
    {
        throw std::invalid_argument("Invalid URL format");
    }

    result.protocol = match[1].str();
    result.host = match[2].str();

    if (match[3].matched)
    {
        result.port = std::stoi(match[3].str());
    }
    else
    {
        if (result.protocol == "ws")
        {
            result.port = 80;
        }
        else if (result.protocol == "wss")
        {
            result.port = 443;
        }
        else
        {
            throw std::invalid_argument("Unsupported protocol: " + result.protocol);
        }
    }

    result.path = match[4].matched ? match[4].str() : "";

    return result;
}

void SignalingClient::setup_context_()
{
    this->logger_->info("Setting up context");

    lws_protocols protocols[] = {
        {"lws-minimal-client", SignalingClient::callback_, 0, 0, 0, nullptr, 0},
        LWS_PROTOCOL_LIST_TERM
    };

    lws_context_creation_info info = {};
    memset(&info, 0, sizeof info);
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = protocols;
    info.timeout_secs = 10;
    info.connect_timeout_secs = 30;
    info.fd_limit_per_thread = 3;
    info.user = this;

    struct lws_context* ctx = lws_create_context(&info);

    if (ctx != nullptr)
    {
        this->ctx_.reset(ctx);
    }
}

void SignalingClient::setup_logger_(std::string logger_name)
{
    this->logger_ = spdlog::create<log_sink>(std::move(logger_name));
}

int SignalingClient::callback_(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len)
{
    lws_context* lws_ctx = lws_get_context(wsi);
    auto* client = static_cast<SignalingClientInterface*>(lws_context_user(lws_ctx));

    switch (reason)
    {
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        if (client)
        {
            spdlog::error("Client {} connection error. Error: {}", in);
        }
        break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
        if (client != nullptr && in != nullptr)
        {
            client->receive(static_cast<const char*>(in), len);
        }
        break;

    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        if (client)
        {
            spdlog::info("Signaling client {} established", client->describe());
        }

        lws_callback_on_writable(wsi);
        break;

    case LWS_CALLBACK_CLIENT_CLOSED:
        if (client)
        {
            spdlog::info("Signaling client {} closed", client->describe());
        }

        break;

    case LWS_CALLBACK_CLIENT_WRITEABLE:
        if (client)
        {
            spdlog::info("Signaling client {} is writable", client->describe());
        }
    default:
        break;
    }

    return 0;
}
