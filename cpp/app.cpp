#include <csignal>

#include "conductor.hpp"
#include "signaling_client.hpp"
#include "peer_connection.hpp"
#include "rtc_base/ssl_adapter.h"

int main(int argc, const char** argv) {
    rtc::InitializeSSL();

    struct lws_context_creation_info info;
    const char *p;
    int n = 0;

    memset(&info, 0, sizeof info);
    lws_cmdline_option_handle_builtin(argc, argv, &info);

    Conductor *conductor = new Conductor();

    std::shared_ptr<SignalingClientInterface> signaling_client = std::make_shared<SignalingClient>(conductor);
    rtc::scoped_refptr<PeerConnection> peer_connection = rtc::make_ref_counted<PeerConnection>(conductor);
    peer_connection->AddRef();
    conductor->set_peer_connection(peer_connection.get());
    conductor->set_signaling_client(signaling_client.get());

    peer_connection->call();
    signaling_client->connect("192.168.0.45", "signaling", 8444);

    std::string line;

    std::thread t([peer_connection]()
    {
        while (true)
        {
            if (peer_connection)
            {
                peer_connection->audio_track_source_->silence();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    while (std::getline(std::cin, line))
    {
        if (line == "exit")
            break;

        signaling_client->send(line);
    }

    rtc::CleanupSSL();
    return 0;
}
