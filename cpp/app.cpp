//
// Created by bimba on 27.06.25.
//

#include "signaling_client.hpp"
#include "peer_connection.hpp"

int main(int argc, const char **argv) {
  struct lws_context_creation_info info = {};
  lws_cmdline_option_handle_builtin(argc, argv, &info);
  PeerConnection peerConnection;

  peerConnection.call();

  SignalingClient sig_client;
  sig_client.connect("192.168.0.21", "signaling",8444);

  while (std::cin.get() != 'q') {

  }
  sig_client.disconnect();
  // sig_client.join();
}