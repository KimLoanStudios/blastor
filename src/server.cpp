#include <SFML/Network.hpp>
#include <iostream>

#include "types.hpp"
#include "event.hpp"

int run_server(int argc __attribute__((unused)), const char* argv[] __attribute__((unused))) {
    std::cout << "Running server!\n";

    int port = 6969;

    sf::UdpSocket sock;
    if (sock.bind(port) != sf::Socket::Done) {
        std::cerr << "Could not bind socket to port " << port << std::endl;
        return 1;
    }

    sock.setBlocking(false);

    f32 tick = 1.f/32.f;
    while (true) {
        sf::Clock clock;

        while (true) {
            sf::Packet pack;
            sf::IpAddress remote_addr;
            unsigned short remote_port;

            auto res = sock.receive(pack, remote_addr, remote_port);
            if (res != sf::Socket::Done)
                break;

            //hadle_packer(pack);
        }

        auto elapsed = clock.restart().asSeconds();
        auto toSleep = std::max(tick - elapsed, 0.f);
        sf::sleep(sf::seconds(toSleep));
    }

    return 0;
}

void handle_packet(sf::Packet& pkt, sf::IpAddress addr, unsigned short port) {
    std::cout << "received packet from" << addr << ":" << port << " length = " << pkt.getDataSize();
}
