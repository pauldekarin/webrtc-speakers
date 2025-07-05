#include "api/peer_connection_interface.h"
#include "api/environment/environment_factory.h"
#include "api/audio/audio_processing.h"
#include "api/create_peerconnection_factory.h"
#include "rtc_base/third_party/sigslot/sigslot.h"
#include "rtc_base/ssl_adapter.h"
#include "rtc_base/socket_address.h"
#include "third_party/abseil-cpp/absl/strings/string_view.h"
#include "rtc_base/physical_socket_server.h"
#include "api/audio/create_audio_device_module.h"
#include "rtc_base/ref_count.h"
#include <sys/socket.h>
#include <chrono>
#include <thread>
#include <string>
#include <iostream>
#include "api/audio_codecs/audio_encoder_factory_template.h"
#include <api/environment/environment.h>
#include "api/audio/audio_device.h"
#include "api/audio/builtin_audio_processing_builder.h"
#include "api/audio/create_audio_device_module.h"
#include "api/audio_codecs/audio_decoder_factory_template.h"
#include "api/audio_codecs/audio_encoder_factory_template.h"
#include "api/audio_codecs/opus/audio_decoder_opus.h"
#include "api/audio_codecs/opus/audio_encoder_opus.h"
#include "api/create_peerconnection_factory.h"
#include "api/enable_media.h"
#include "api/environment/environment_factory.h"
#include "api/peer_connection_interface.h"
#include "api/rtc_event_log/rtc_event_log_factory.h"
#include "api/video_codecs/video_decoder_factory_template.h"
#include "api/video_codecs/video_decoder_factory_template_dav1d_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_open_h264_adapter.h"
#include "api/video_codecs/video_encoder_factory_template.h"
#include "api/video_codecs/video_encoder_factory_template_libaom_av1_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_open_h264_adapter.h"
#include "rtc_base/thread.h"
#include <api/peer_connection_interface.h>
#include <api/media_stream_interface.h>
#include <api/create_peerconnection_factory.h>

#include "IXNetSystem.h"
#include "api/audio_codecs/audio_decoder_factory.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_decoder_factory_template.h"
#include "api/video_codecs/video_decoder_factory_template_dav1d_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_open_h264_adapter.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "api/video_codecs/video_encoder_factory_template.h"
#include "api/video_codecs/video_encoder_factory_template_libaom_av1_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_open_h264_adapter.h"
#include "api/video/i420_buffer.h"
#include "api/video/video_frame_buffer.h"




class Client: public webrtc::PeerConnectionObserver, public webrtc::CreateSessionDescriptionObserver{
public:
void AddRef() const override{
}

webrtc::RefCountReleaseStatus Release() const override{
    return webrtc::RefCountReleaseStatus::kDroppedLastRef;
}

    void OnSuccess(webrtc::SessionDescriptionInterface *desc) override{
        std::cout << "SDP event!" << std::endl;
    }

    void OnFailure(webrtc::RTCError error) override{
        std::cout << "SDP event!" << std::endl;
    }

    ~Client() override{
    }

    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override{
    }

    void OnAddStream(webrtc::scoped_refptr<webrtc::MediaStreamInterface>) override{
    }

    void OnRemoveStream(webrtc::scoped_refptr<webrtc::MediaStreamInterface>) override{
    }

    void OnDataChannel(webrtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override{
    }

    void OnRenegotiationNeeded() override{
    }

    void OnNegotiationNeededEvent(uint32_t) override{
    }

    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState) override{
    }

    void OnStandardizedIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState) override{
    }

    void OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState) override{
    }

    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override{
    }

    void OnIceCandidate(const webrtc::IceCandidate *candidate) override{
    }

    void OnIceCandidateError(const std::string &, int, const std::string &, int, const std::string &) override{
    }

    void OnIceCandidatesRemoved(const std::vector<webrtc::Candidate> &) override{
    }

    void OnIceConnectionReceivingChange(bool) override{
    }

    void OnIceSelectedCandidatePairChanged(const webrtc::CandidatePairChangeEvent &) override{
    }

    void OnAddTrack(webrtc::scoped_refptr<webrtc::RtpReceiverInterface>,
        const std::vector<webrtc::scoped_refptr<webrtc::MediaStreamInterface>> &) override{
        }

    void OnTrack(webrtc::scoped_refptr<webrtc::RtpTransceiverInterface>) override{
    }

    void OnRemoveTrack(webrtc::scoped_refptr<webrtc::RtpReceiverInterface>) override{
    }

    void OnInterestingUsage(int) override{
    }

    Client() {
        webrtc::InitializeSSL();
        webrtc::PeerConnectionFactoryDependencies media_deps;
        webrtc::EnvironmentFactory environment_factory;

        // Создание и запуск потоков
        auto signaling_thread = webrtc::Thread::CreateWithSocketServer();
        signaling_thread->Start();
        auto worker_thread = webrtc::Thread::Create();
        worker_thread->Start();

        // Создание фабрики на основном потоке
        webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory;
        {
            factory = webrtc::CreatePeerConnectionFactory(
                nullptr, worker_thread.get(), signaling_thread.get(),
                webrtc::scoped_refptr<webrtc::AudioDeviceModule>(webrtc::make_ref_counted<AudioCaptureModule>()),
                webrtc::CreateBuiltinAudioEncoderFactory(),
                webrtc::CreateBuiltinAudioDecoderFactory(),
                std::make_unique<webrtc::VideoEncoderFactoryTemplate<
                    webrtc::LibvpxVp8EncoderTemplateAdapter,
                    webrtc::LibvpxVp9EncoderTemplateAdapter,
                    webrtc::OpenH264EncoderTemplateAdapter,
                    webrtc::LibaomAv1EncoderTemplateAdapter>>(),
                std::make_unique<webrtc::VideoDecoderFactoryTemplate<
                    webrtc::LibvpxVp8DecoderTemplateAdapter,
                    webrtc::LibvpxVp9DecoderTemplateAdapter,
                    webrtc::OpenH264DecoderTemplateAdapter,
                    webrtc::Dav1dDecoderTemplateAdapter>>(),
                nullptr /* audio_mixer */, nullptr);
        }

        if (!factory) {
            std::cout << "Failed to create PeerConnectionFactory" << std::endl;
            return;
        }
        std::cout << "PeerConnectionFactory created" << std::endl;
        webrtc::PeerConnectionInterface::RTCConfiguration configuration;
        webrtc::PeerConnectionDependencies dependencies(this);

        auto result = factory->CreatePeerConnectionOrError(configuration, std::move(dependencies));
        if (!result.ok()) {
            std::cout << "error!" << std::endl;
        }

        webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_interface = result.value();
        std::cout << "ok" << std::endl;

        webrtc::scoped_refptr<AudioSource> audio_track_source = webrtc::make_ref_counted<AudioSource>();
        webrtc::scoped_refptr<VideoTrackSource> video_track_source = webrtc::make_ref_counted<VideoTrackSource>();

        webrtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(factory->CreateAudioTrack("audio_label", audio_track_source.get()));

        auto r = peer_connection_interface->AddTrack(audio_track, {"stream_id"});
        if (!r.ok()) {
            std::cout << "error add audio track!" << std::endl;
        }

        peer_connection_interface->CreateOffer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
        webrtc::CleanupSSL();
    }
};
#include "pc/test/fake_audio_capture_module.h"
using namespace webrtc;
#include <libwebsockets.h>
#include <string.h>
#include <signal.h>

/*
 * This represents your object that "contains" the client connection and has
 * the client connection bound to it
 */

static struct my_conn {
        lws_sorted_usec_list_t	sul;	     /* schedule connection retry */
        struct lws		*wsi;	     /* related wsi if any */
        uint16_t		retry_count; /* count of consequetive retries */
} mco;

static struct lws_context *context;
static int interrupted, port = 443, ssl_connection = LCCSCF_USE_SSL;
static const char *server_address = "echo.websocket.org",
                  *pro = "dumb-increment-protocol";

/*
 * The retry and backoff policy we want to use for our client connections
 */

static const uint32_t backoff_ms[] = { 1000, 2000, 3000, 4000, 5000 };

static const lws_retry_bo_t retry = {
        .retry_ms_table			= backoff_ms,
        .retry_ms_table_count		= LWS_ARRAY_SIZE(backoff_ms),
        .conceal_count			= LWS_ARRAY_SIZE(backoff_ms),

        .secs_since_valid_ping		= 3,  /* force PINGs after secs idle */
        .secs_since_valid_hangup	= 10, /* hangup after secs idle */

        .jitter_percent			= 20,
};

/*
 * Scheduled sul callback that starts the connection attempt
 */

static void
connect_client(lws_sorted_usec_list_t *sul)
{
        struct my_conn *mco = lws_container_of(sul, struct my_conn, sul);
        struct lws_client_connect_info i;

        memset(&i, 0, sizeof(i));

        i.context = context;
        i.port = port;
        i.address = server_address;
        i.path = "/signaling";
        i.host = i.address;
        i.origin = i.address;
        i.ssl_connection = ssl_connection;
        i.protocol = pro;
        i.local_protocol_name = "lws-minimal-client";
        i.pwsi = &mco->wsi;
        i.retry_and_idle_policy = &retry;
        i.userdata = mco;

    lwsl_user("Connection to: %s%d%s Host: %s", server_address, i.port, i.path);


        if (!lws_client_connect_via_info(&i))
                /*
                 * Failed... schedule a retry... we can't use the _retry_wsi()
                 * convenience wrapper api here because no valid wsi at this
                 * point.
                 */
                if (lws_retry_sul_schedule(context, 0, sul, &retry,
                                           connect_client, &mco->retry_count)) {
                        lwsl_err("%s: connection attempts exhausted\n", __func__);
                        interrupted = 1;
                }
}

static int
callback_minimal(struct lws *wsi, enum lws_callback_reasons reason,
                 void *user, void *in, size_t len)
{
        struct my_conn *mco = (struct my_conn *)user;

        switch (reason) {

        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
                lwsl_err("CLIENT_CONNECTION_ERROR: %s\n",
                         in ? (char *)in : "(null)");
                goto do_retry;
                break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
                lwsl_hexdump_notice(in, len);
                break;

        case LWS_CALLBACK_CLIENT_ESTABLISHED:
                lwsl_user("%s: established\n", __func__);
                break;

        case LWS_CALLBACK_CLIENT_CLOSED:
                goto do_retry;

        default:
                break;
        }

        return lws_callback_http_dummy(wsi, reason, user, in, len);

do_retry:
        /*
         * retry the connection to keep it nailed up
         *
         * For this example, we try to conceal any problem for one set of
         * backoff retries and then exit the app.
         *
         * If you set retry.conceal_count to be larger than the number of
         * elements in the backoff table, it will never give up and keep
         * retrying at the last backoff delay plus the random jitter amount.
         */
        if (lws_retry_sul_schedule_retry_wsi(wsi, &mco->sul, connect_client,
                                             &mco->retry_count)) {
                lwsl_err("%s: connection attempts exhausted\n", __func__);
                interrupted = 1;
        }

        return 0;
}

static const struct lws_protocols protocols[] = {
        { "lws-minimal-client", callback_minimal, 0, 0, 0, NULL, 0 },
        LWS_PROTOCOL_LIST_TERM
};

static void
sigint_handler(int sig)
{
        interrupted = 1;
}

int main(int argc, const char **argv)
{
        struct lws_context_creation_info info;
        const char *p;
        int n = 0;

        signal(SIGINT, sigint_handler);
        memset(&info, 0, sizeof info);
        lws_cmdline_option_handle_builtin(argc, argv, &info);

        lwsl_user("LWS minimal ws client\n");

        info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
        info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
        info.protocols = protocols;

#if defined(LWS_WITH_MBEDTLS) || defined(USE_WOLFSSL)
        /*
         * OpenSSL uses the system trust store.  mbedTLS has to be told which
         * CA to trust explicitly.
         */
        info.client_ssl_ca_filepath = "./libwebsockets.org.cer";
#endif

        if ((p = lws_cmdline_option(argc, argv, "--protocol")))
                pro = p;

        if ((p = lws_cmdline_option(argc, argv, "-s")))
                server_address = p;

        if ((p = lws_cmdline_option(argc, argv, "-p")))
                port = atoi(p);

        if (lws_cmdline_option(argc, argv, "-n"))
                ssl_connection &= ~LCCSCF_USE_SSL;

        if (lws_cmdline_option(argc, argv, "-j"))
                ssl_connection |= LCCSCF_ALLOW_SELFSIGNED;

        if (lws_cmdline_option(argc, argv, "-k"))
                ssl_connection |= LCCSCF_ALLOW_INSECURE;

        if (lws_cmdline_option(argc, argv, "-m"))
                ssl_connection |= LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;

        if (lws_cmdline_option(argc, argv, "-e"))
                ssl_connection |= LCCSCF_ALLOW_EXPIRED;

        info.fd_limit_per_thread = 1 + 1 + 1;

        context = lws_create_context(&info);
        if (!context) {
                lwsl_err("lws init failed\n");
                return 1;
        }

        /* schedule the first client connection attempt to happen immediately */
        lws_sul_schedule(context, 0, &mco.sul, connect_client, 1);

        while (n >= 0 && !interrupted)
                n = lws_service(context, 0);

        lws_context_destroy(context);
        lwsl_user("Completed\n");

        return 0;
}

class PeerConnectionClient: public sigslot::has_slots<>{

private:
    int id_;

    webrtc::SocketAddress socket_addr_;
    std::string client_name_;
    std::unique_ptr<webrtc::Socket> control_socket_;
    std::unique_ptr<webrtc::Socket> hanging_get_;

public:
    PeerConnectionClient(){

    }

    ~PeerConnectionClient(){

    }

    int id() const{
        return this->id_;
    }

    bool is_connected() const{
        return this->id_ != -1;
    }

    void connect(const std::string &address, int port, const std::string &client_name) {
        absl::string_view address_view(address.c_str(), address.size());

        this->socket_addr_.SetIP(address_view);

        this->socket_addr_.SetPort(port);

        this->client_name_ = client_name;

        if(this->socket_addr_.IsUnresolvedIP()){
            std::cout << "connect() ip is unresolved" << std::endl;
            std::exit(EXIT_FAILURE);
        }else{
            std::cout << this->socket_addr_.ipaddr().ToString() << ":" << this->socket_addr_.port() << std::endl;

            this->control_socket_.reset(this->create_socket(this->socket_addr_.family()));
            webrtc::Socket* socket = this->control_socket_.get();

            int res = socket->Connect(this->socket_addr_);
            if (res == SOCKET_ERROR) {
                std::cout << "connect() failed" << std::endl;
                std::exit(EXIT_FAILURE);
            }
            this->connect_signals();
        }
    }

    void connect_signals(){
        this->control_socket_->SignalCloseEvent.connect(this, &PeerConnectionClient::on_close);
        this->control_socket_->SignalConnectEvent.connect(this, &PeerConnectionClient::on_connect);
    }

    void on_close(webrtc::Socket* socket, int err){
        std::cout << "Close!\n";
    }

    void on_connect(webrtc::Socket* socket){
        std::ostringstream message;
        this->client_name_ = "test";
        message << "GET /sign_in?"<< this->client_name_ <<" HTTP/1.0\r\n\r\n";
        int sent_bytes =  socket->Send(message.str().c_str(), message.str().size());
        if (sent_bytes != message.str().size()) {
            std::cout << "on_connect() failed" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        std::cout << "Connect!\n";
    }

    webrtc::Socket* create_socket(int family) {
        static bool wrapped_current_thread = false;
        if (!wrapped_current_thread) {
            wrapped_current_thread = webrtc::ThreadManager::Instance()->WrapCurrentThread() != nullptr;
        }

        webrtc::Thread* thread = webrtc::Thread::Current();
        if (thread == nullptr) {
            std::cerr << "webrtc::Thread::Current() is nullptr";
            std::exit(EXIT_FAILURE);
        }
        webrtc::Socket* socket = thread->socketserver()->CreateSocket(family, SOCK_STREAM);
        return socket;
    }
};

class Server: public webrtc::PhysicalSocketServer {
    public:

bool Wait(webrtc::TimeDelta delta, bool process_io) override {
    std::cout << "wait!\n";
    return webrtc::PhysicalSocketServer::Wait(webrtc::TimeDelta::Millis(100), process_io);
}

    void set_peer_connection_client(PeerConnectionClient* client) {
        this->peer_connection_client_ = client;
    }

private:
    PeerConnectionClient* peer_connection_client_;

};
//
// #include "websocketpp/client.hpp"
// #include "websocketpp/config/asio_no_tls_client.hpp"
// #include "websocketpp/config/asio_client.hpp"
// //
// // typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
// //
// // bool verify_certificate(const char *hostname, bool preverified, boost::asio::ssl::verify_context &ctx) {
// //     return true;
// // }
// //
// websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> on_tls_init(const char *hostname, websocketpp::connection_hdl hdl) {
//     websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr = websocketpp::lib::make_shared<websocketpp::lib::asio::ssl::context>(boost::asio::ssl::context::tlsv13);
//     context_ptr->set_default_verify_paths();
//     context_ptr->set_verify_mode(boost::asio::ssl::verify_none);
//     return context_ptr;
// }
// //
// // class connection_metadata {
// // public:
// //     typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;
// //
// //     connection_metadata(int id, websocketpp::connection_hdl hdl, std::string uri):
// //     id_(id), hdl_(hdl), uri_(uri),status_("Connecting"), server_("N/A")
// //     {
// //
// //     }
// //
// //     void on_open(client *client, websocketpp::connection_hdl hdl) {
// //         std::cout << "Opened!" << std::endl;
// //         this->status_ = "Open";
// //
// //         client::connection_ptr con = client->get_con_from_hdl(hdl);
// //         this->server_ = con->get_response_header("Server");
// //     }
// //
// //     void on_fail(client *client, websocketpp::connection_hdl hdl) {
// //         std::cout << "Failed!" << std::endl;
// //         this->status_ = "Failed";
// //
// //         client::connection_ptr con = client->get_con_from_hdl(hdl);
// //         this->server_ = con->get_response_header("Server");
// //         this->error_reason_ = con->get_ec().message();
// //     }
// //
// // private:
// //     int id_;
// //     websocketpp::connection_hdl hdl_;
// //     std::string status_;
// //     std::string uri_;
// //     std::string server_;
// //     std::string error_reason_;
// // };
// //
// // class websocket_endpoint {
// // public:
// //     websocket_endpoint() {
// //         this->m_endpoint_.clear_access_channels(websocketpp::log::alevel::all);
// //         this->m_endpoint_.clear_error_channels(websocketpp::log::elevel::all);
// //
// //         this->m_endpoint_.set_access_channels(websocketpp::log::alevel::all);
// //         this->m_endpoint_.set_error_channels(websocketpp::log::elevel::all);
// //
// //         this->m_endpoint_.init_asio();
// //         this->m_endpoint_.set_message_handler(websocketpp::lib::bind(&websocket_endpoint::on_message, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
// //
// //         this->m_endpoint_.start_perpetual();
// //     }
// //
// //     void on_message(websocketpp::connection_hdl hdl, client::message_ptr message) {
// //             std::cout << "Received: " << message << std::endl;
// //     }
// //
// //     ~websocket_endpoint() {
// //         this->close();
// //     }
// //
// //     void close() {
// //         this->m_endpoint_.stop_perpetual();
// //     }
// //
// //     int connect(const std::string &uri) {
// //         this->m_endpoint_.set_tls_init_handler(websocketpp::lib::bind(&on_tls_init, uri.c_str(), websocketpp::lib::placeholders::_1));
// //
// //         websocketpp::lib::error_code error_code;
// //
// //         client::connection_ptr con = this->m_endpoint_.get_connection(uri, error_code);
// //
// //         if (error_code) {
// //             std::cerr << "connect() failed: " << error_code.message() << std::endl;
// //             return -1;
// //         }
// //
// //         int id = this->next_id_++;
// //         connection_metadata::ptr metadata_ptr(new connection_metadata(id, con->get_handle(), uri));
// //
// //         con->set_open_handler(websocketpp::lib::bind(
// //             &connection_metadata::on_open,
// //             metadata_ptr,
// //             &this->m_endpoint_,
// //             websocketpp::lib::placeholders::_1));
// //
// //         con->set_fail_handler(websocketpp::lib::bind(
// //             &connection_metadata::on_fail,
// //             metadata_ptr,
// //             &this->m_endpoint_,
// //             websocketpp::lib::placeholders::_1));
// //
// //         this->m_endpoint_.connect(con);
// //
// //         this->m_endpoint_.get_alog().write(websocketpp::log::alevel::app, "connect");
// //         return  id;
// //     }
// //
// // private:
// //     client m_endpoint_;
// //     websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread_;
// //
// //     int next_id_ = 0;
// // };
// //
// //
// // int main(int argc, char** argv) {
// //     websocket_endpoint endpoint;
// //
// //     endpoint.connect("wss://192.168.0.21:8444/signaling");
// //
// //     std::thread input_thread([&endpoint]() {
// //          bool done = false;
// //          while (!done) {
// //              std::string line;
// //              std::getline(std::cin, line);
// //              if (line == "q") {
// //                  done = true;
// //              }
// //          }
// //          endpoint.close();
// //      });
// //
// //     input_thread.join();
// //
// // }
//
// #include <cstdlib>
// #include <iostream>
// #include <map>
// #include <string>
// #include <sstream>
//
// #include <websocketpp/config/asio_client.hpp>
// #include <boost/asio.hpp>
// #include <boost/asio/ssl.hpp>
// #include <boost/asio/ssl/context.hpp>
// #include <websocketpp/client.hpp>
//
// #include <websocketpp/common/thread.hpp>
// #include <websocketpp/common/memory.hpp>
//
// typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
//
// class connection_metadata {
//     private:
//         int m_id;
//         websocketpp::connection_hdl m_hdl;
//         std::string m_status;
//         std::string m_uri;
//         std::string m_server;
//         std::string m_error_reason;
//
//     public:
//         typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;
//
//         connection_metadata(int id, websocketpp::connection_hdl hdl, std::string uri):
//         m_id(id),
//         m_hdl(hdl),
//         m_status("Connecting"),
//         m_uri(uri),
//         m_server("N/A")
//         {}
//
//         int get_id(){return m_id;}
//         websocketpp::connection_hdl get_hdl(){ return m_hdl;}
//         std::string get_status(){return m_status;}
//
//         void on_open(client * c, websocketpp::connection_hdl hdl){
//             m_status = "Open";
//
//             client::connection_ptr con = c->get_con_from_hdl(hdl);
//             m_server = con->get_response_header("Server");
//         }
//
//         void on_fail(client * c, websocketpp::connection_hdl hdl){
//             m_status = "Failed";
//
//             client::connection_ptr con = c->get_con_from_hdl(hdl);
//             m_server = con->get_response_header("Server");
//             m_error_reason = con->get_ec().message();
//         }
//
//         void on_close(client * c, websocketpp::connection_hdl hdl){
//             m_status = "Closed";
//
//             client::connection_ptr con = c->get_con_from_hdl(hdl);
//             std::stringstream s;
//             s << "Close code: " << con->get_remote_close_code() << "("
//               << websocketpp::close::status::get_string(con->get_remote_close_code())
//               << "), Close reason: " << con->get_remote_close_reason();
//
//             m_error_reason = s.str();
//         }
//
//         friend std::ostream &operator<< (std::ostream &out, connection_metadata const &data);
// };
//
// std::ostream &operator<< (std::ostream &out, connection_metadata const &data){
//     out << "> URI: " << data.m_uri << "\n"
//         << "> Status: " << data.m_status << "\n"
//         << "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
//         << "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason);
//
//     return out;
// }
//
// class websocket_endpoint {
//     private:
//         typedef std::map<int,connection_metadata::ptr> con_list;
//
//         client m_endpoint;
//         websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
//
//         con_list m_connection_list;
//         int m_next_id;
//
//     public:
//         websocket_endpoint(): m_next_id(0) {
//             m_endpoint.set_access_channels(websocketpp::log::alevel::all);
//             m_endpoint.set_error_channels(websocketpp::log::elevel::all);
//
//             m_endpoint.init_asio();
//             m_endpoint.start_perpetual();
//
//             m_thread.reset(new websocketpp::lib::thread(&client::run, &m_endpoint));
//         }
//
//         ~websocket_endpoint() {
//             m_endpoint.stop_perpetual();
//
//             for (con_list::const_iterator it = m_connection_list.begin(); it != m_connection_list.end(); ++it) {
//                 if (it->second->get_status() != "Open") {
//                     continue;
//                 }
//
//                 std::cout << "> Closing connection " << it->second->get_id() << std::endl;
//
//                 websocketpp::lib::error_code ec;
//                 m_endpoint.close(it->second->get_hdl(), websocketpp::close::status::going_away, "", ec);
//                 if (ec) {
//                     std::cout << "> Error closing connection " << it->second->get_id() << ": "
//                             << ec.message() << std::endl;
//                 }
//             }
//
//             m_thread->join();
//         }
//
//         int connect (std::string const &uri) {
//             m_endpoint.set_tls_init_handler([](websocketpp::connection_hdl){
//                                         boost::asio::ssl::context context(boost::asio::ssl::context::tlsv1);
//                                         return websocketpp::lib::make_shared<boost::asio::ssl::context>(std::move(context));
//                                         });
//
//             websocketpp::lib::error_code ec;
//
//             client::connection_ptr con = m_endpoint.get_connection(uri, ec);
//
//             if(ec){
//                 std::cout << "Connection initialization error: " << ec.message() << std::endl;
//                 return -1;
//             }
//
//             int new_id = m_next_id++;
//             connection_metadata::ptr metadata_ptr(new connection_metadata(new_id, con->get_handle(), uri));
//             m_connection_list[new_id] = metadata_ptr;
//
//             con->set_open_handler(websocketpp::lib::bind(
//                                   &connection_metadata::on_open,
//                                   metadata_ptr,
//                                   &m_endpoint,
//                                   websocketpp::lib::placeholders::_1
//                                   ));
//
//             con->set_fail_handler(websocketpp::lib::bind(
//                                   &connection_metadata::on_fail,
//                                   metadata_ptr,
//                                   &m_endpoint,
//                                   websocketpp::lib::placeholders::_1
//                                   ));
//             con->set_close_handler(websocketpp::lib::bind(
//                                    &connection_metadata::on_close,
//                                    metadata_ptr,
//                                    &m_endpoint,
//                                    websocketpp::lib::placeholders::_1
//                                   ));
//
//             m_endpoint.connect(con);
//
//             return new_id;
//         }
//
//         connection_metadata::ptr get_metadata(int id) const {
//             con_list::const_iterator metadata_it = m_connection_list.find(id);
//             if (metadata_it == m_connection_list.end()) {
//                 return connection_metadata::ptr();
//             } else {
//                 return metadata_it->second;
//             }
//         }
//
//         void close(int id, websocketpp::close::status::value code, std::string reason) {
//             websocketpp::lib::error_code ec;
//
//             con_list::iterator metadata_it = m_connection_list.find(id);
//             if (metadata_it == m_connection_list.end()) {
//                 std::cout << "> No connection found with id " << id << std::endl;
//                 return;
//             }
//
//             m_endpoint.close(metadata_it->second->get_hdl(), code, "", ec);
//             std::cout << "Connection closed with reason: " << reason << std::endl;
//         }
// };
//
// int main(){
//     bool done = false;
//     std::string input;
//     websocket_endpoint endpoint;
//
//     while(!done) {
//         std::cout << "Enter Command: ";
//         std::getline(std::cin, input);
//
//         if(input == "quit"){
//             done = true;
//         }
//         else if(input == "help"){
//             std::cout << "\nCommand List:\n"
//             << "help: Displays this help text\n"
//             << "quit: exits the program\n"
//             << "connect <URI>: creates a connection with the given URI\n"
//             << "close <id> <code (optional)> <reason (optional)>: closes the connection with the given id with optionally specifiable exit_code and/or reason\n"
//             << "show <id>: Gets the metadata of the connection with the given id\n"
//             << std::endl;
//         }
//         else if (input.substr(0,7) == "connect") {
//             int id = endpoint.connect(input.substr(8));
//             if (id != -1) {
//                 std::cout << "> Created connection with id " << id << std::endl;
//             }
//         }
//         else if (input.substr(0,4) == "show") {
//             int id = atoi(input.substr(5).c_str());
//
//             connection_metadata::ptr metadata = endpoint.get_metadata(id);
//             if (metadata) {
//                 std::cout << *metadata << std::endl;
//             } else {
//                 std::cout << "> Unknown connection id " << id << std::endl;
//             }
//         }
//         else if(input.substr(0, 5) == "close"){
//             std::stringstream ss(input);
//
//             std::string cmd;
//             int id;
//             int close_code = websocketpp::close::status::normal;
//             std::string reason;
//
//             ss >> cmd >> id >> close_code;
//             std::getline(ss, reason);
//
//             endpoint.close(id, close_code, reason);
//         }
//         else{
//             std::cout << "Unrecognized command" << std::endl;
//         }
//     }
//     return 0;
// }
//
// // int main(int argc, char** argv){
// //     webrtc::InitializeSSL();
// //     Server server;
// //     webrtc::AutoSocketServerThread server_thread(&server);
// //     PeerConnectionClient peerConnection;
// //
// //     peerConnection.connect("127.0.0.1", 8080, "test");
// //
// //     server_thread.Run();
// //
// //     webrtc::CleanupSSL();
// // }ifr_qlen