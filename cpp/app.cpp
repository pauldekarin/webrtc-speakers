#include <csignal>

#include "conductor.hpp"
#include "signaling_client.hpp"
#include "peer_connection.hpp"

int main(int argc, const char** argv) {
    struct lws_context_creation_info info;
    const char *p;
    int n = 0;

    memset(&info, 0, sizeof info);
    lws_cmdline_option_handle_builtin(argc, argv, &info);

    Conductor *conductor = new Conductor();

    std::shared_ptr<SignalingClient> signaling_client = std::make_shared<SignalingClient>(conductor);
    std::shared_ptr<PeerConnection> peer_connection = std::make_shared<PeerConnection>(conductor);

    conductor->set_peer_connection(peer_connection.get());
    conductor->set_signaling_client(signaling_client.get());

    peer_connection->call();
    signaling_client->connect("192.168.0.45", "signaling", 8444);

    std::string line;

    while (std::getline(std::cin, line))
    {
        if (line == "exit")
            break;

        signaling_client->send(line);
    }
    return 0;
}