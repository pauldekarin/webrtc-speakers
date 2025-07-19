//
// Created by bimba on 7/19/25.
//

#include "peer_connection.hpp"

int main(int argc, char **argv)
{
    rtc::scoped_refptr<PeerConnection> pc = rtc::make_ref_counted<PeerConnection>();
    pc->call();

    char c;
    std::cin >> c;

}
