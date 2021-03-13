#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <cstring>

#include "types.hpp"
#include "config.hpp"
#include "event.hpp"
#include "gamestate.hpp"

using SockAddr = std::pair<sf::IpAddress, u16>;

struct PeerInfo {
    std::string name;
    u64 id;
};

struct Peers {
    std::map<SockAddr, PeerInfo> peers;
    std::map<u64, SockAddr> id_to_addr;

    u64 create_peer(SockAddr addr, const std::string& name) {
        // TODO change this to smth better
        u64 id = std::hash<std::string>{}(name);
        auto peer = PeerInfo {
            .name = name,
            .id = id,
        };

        peers[addr] = peer;
        id_to_addr[id] = addr;

        return id;
    }
};

struct Server {
    std::vector<Event> events;
    sf::UdpSocket sock;
    u16 port;
    GameState game_state;
    Peers peers;

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

                handle_packet(pack, {remote_addr, remote_port});
            }

            auto elapsed = clock.restart().asSeconds();
            auto toSleep = std::max(TICK_TIME - elapsed, 0.f);
            sf::sleep(sf::seconds(toSleep));
        }
    }

    void handle_packet(sf::Packet& pkt, SockAddr addr) {
        std::cout <<
            "received packet from" << addr.first << ":" << addr.second <<
            " length = " << pkt.getDataSize();

        Event event;
        pkt >> event;

        switch (event.content.index()) {
            case 0:
                break;
        }
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

