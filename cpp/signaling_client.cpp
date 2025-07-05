//
// Created by bimba on 27.06.25.
//

#include "signaling_client.hpp"

SignalingClient::SignalingClient():
ctx_(nullptr, [](struct lws_context *ctx){if (ctx != nullptr) lws_context_destroy(ctx);})
{
       lws_protocols protocols[] = {
           { "lws-minimal-client", SignalingClient::callback, 0, 0, 0, nullptr, 0 },
            LWS_PROTOCOL_LIST_TERM
      };

     lws_context_creation_info info = {};

     info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
     info.port = CONTEXT_PORT_NO_LISTEN;
     info.protocols = protocols;
     info.fd_limit_per_thread = 3;

     struct lws_context *ctx = lws_create_context(&info);

     if(ctx != nullptr){
           this->ctx_.reset(ctx);
       }
}

SignalingClient::~SignalingClient(){
    this->destroy();
  }



void SignalingClient::connect(const std::string &address, const std::string &path, int port){
    lwsl_user("Trying to connect %s:%d", address.c_str(), port );

    struct lws_client_connect_info info = {};
    info.address = address.c_str();
    info.port = port;
    info.path = ("/" + path).c_str();
    info.host = address.c_str();
    info.context = this->ctx_.get();
    info.ssl_connection = LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED | LCCSCF_ALLOW_INSECURE | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;

    struct lws* conn = lws_client_connect_via_info(&info);

    if(conn == nullptr){
          lwsl_err("failed to connect to server\n");
          return;
      }

    this->lws_.reset(conn);

      lwsl_user("successfully connected to lws server\n");

    this->service_thread_ = std::thread(&SignalingClient::loop_, this);
}

void SignalingClient::send(const char *p_str) {
}

void SignalingClient::disconnect() {
    this->running_.store(false);
    this->service_thread_.join();
    this->destroy();
}

void SignalingClient::destroy() {
    if (this->ctx_) {
        lws_context_destroy(this->ctx_.get());
        this->ctx_.reset(nullptr);
    }
}

void SignalingClient::join() {
    if (this->service_thread_.joinable()) {
        this->service_thread_.join();
    }
}

void SignalingClient::loop_() {
    this->running_.store(true);

    while (this->running_.load() && this->ctx_) {
        if (lws_service(this->ctx_.get(), 0) < 0) {
            lwsl_err("lws_service failed\n");
        }
    }
}

int SignalingClient::callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len){
    struct my_conn *mco = (struct my_conn *)user;

    switch (reason) {

        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            lwsl_err("CLIENT_CONNECTION_ERROR: %s\n",
                     in ? (char *)in : "(null)");
        break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            lwsl_hexdump_notice(in, len);
        break;

        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            lwsl_user("%s: established\n", __func__);
        break;

        case LWS_CALLBACK_CLIENT_CLOSED:
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE: {

        }
        default:
            break;
    }

    return lws_callback_http_dummy(wsi, reason, user, in, len);
}
