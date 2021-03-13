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
    sf::UdpSocket sock;
    u16 port;

    GameState game_state;
    std::vector<Event> events;
    Peers peers;

    u32 tick = 0;

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

            flush_events();

            auto elapsed = clock.restart().asSeconds();
            auto toSleep = std::max(TICK_TIME - elapsed, 0.f);
            sf::sleep(sf::seconds(toSleep));

            tick++;
        }
    }

    void handle_packet(sf::Packet& pkt, SockAddr addr) {
        std::cout <<
            "received packet from" << addr.first << ":" << addr.second <<
            " length = " << pkt.getDataSize();

        Event event;
        pkt >> event;

        std::visit([&](auto &&arg) -> void {
                handle_event(arg, addr);
        }, event.content);
    }

    void handle_event(Hello h, SockAddr addr) {
        std::cout << "received hello message\n" <<
            "   username = " << h.username << "\n" <<
            "   addr = " << addr.first << ":" << addr.second << std::endl;

        auto id = peers.create_peer(addr, h.username);

        std::cout << "  assigning id" << id << std::endl;


        auto response = HelloResponse {
            .player_id = id
        };

        auto event = Event {
            .tick = 0,
            .content = response,
        };

        send_event(event, addr);
    }

    void handle_event(HelloResponse h, SockAddr addr) {
        std::cout << "received hello response\n" <<
            "   addr = " << addr.first << ":" << addr.second << std::endl;
    }

    void handle_event(PlayerPos pp, SockAddr addr) {
        auto event = Event {
            .tick = tick,
            .content = pp,
        };

        events.push_back(event);
    }

    void handle_event(BulletShot h, SockAddr addr) {
    }

    void send_event(Event event, SockAddr to) {
        sf::Packet pkt;
        pkt << event;
        sock.send(pkt, to.first, to.second);
    }

    void flush_events() {
        for (auto &peer : peers.peers) {
            for (auto &event : events) {
                send_event(event, peer.first);
            }
        }

        events.clear();
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

