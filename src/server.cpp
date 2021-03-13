#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <cstring>

#include "types.hpp"
#include "config.hpp"
#include "event.hpp"


struct Server {
    std::vector<Event> events;
    sf::UdpSocket sock;
    u16 port;

    Server(u16 _port) : port(_port) {}

    void run() {
        if (sock.bind(port) != sf::Socket::Done) {
            std::cerr << "Could not bind socket to port " << port << std::endl;
            return;
        }

        sock.setBlocking(false);

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
            auto toSleep = std::max(TICK_TIME - elapsed, 0.f);
            sf::sleep(sf::seconds(toSleep));
        }
    }

    void handle_packet(sf::Packet& pkt, sf::IpAddress addr, unsigned short remote_port) {
        std::cout <<
            "received packet from" << addr << ":" << remote_port <<
            " length = " << pkt.getDataSize();

    }
};


int run_server(int argc, const char* argv[]) {
    u16 port = SERVER_PORT;

    if (argc == 2 && strcmp(argv[0], "--port") == 0) {
        port = std::stoi(argv[1]);
    }

    std::cout << "Running server!\n";
    std::cout << "port = " << port << std::endl;

    Server srv(port);
    srv.run();

    return 0;
}

