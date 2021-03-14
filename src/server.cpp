#include <SFML/Network.hpp>
#include <iostream>
#include <cmath>
#include <functional>
#include <vector>
#include <span>
#include <cstring>

#include "types.hpp"
#include "config.hpp"
#include "event.hpp"
#include "gamestate.hpp"

using SockAddr = std::pair<sf::IpAddress, u16>;

struct PeerInfo {
    std::string name;
    u64 id;
    u32 tick_of_last_event;
};

struct Peers {
    std::map<SockAddr, PeerInfo> peers;
    std::map<u64, SockAddr> id_to_addr;

    using PeerRemovalCallback = std::function<void(const PeerInfo&)>;

    u64 create_peer(SockAddr addr, const std::string& name, u32 tick) {
        // TODO change this to smth better
        u64 id = std::max(std::hash<std::string>{}(name) + addr.second, size_t(1));
        auto peer = PeerInfo {
            .name = name,
            .id = id,
            .tick_of_last_event = tick,
        };

        peers[addr] = peer;
        id_to_addr[id] = addr;

        return id;
    }

    void remove_inactive_peers(u32 tick, PeerRemovalCallback on_removal) {
        for (auto it = peers.cbegin(); it != peers.cend() /* not hoisted */; /* no increment */) {
            auto peer_info = it->second;

            bool must_delete = false;
            if (peer_info.tick_of_last_event + max_inactive_ticks < tick) {
                must_delete = true;
            }


            if (must_delete) {
                id_to_addr.erase(peer_info.id);
                on_removal(peer_info);
                peers.erase(it++);    // or "it = m.erase(it)" since C++11
            } else {
                ++it;
            }
        }
    }

    void keep_peer_alive(u32 tick, u64 peer_id) {
        if (id_to_addr.find(peer_id) != id_to_addr.end()) {
            peers[id_to_addr[peer_id]].tick_of_last_event = tick;
        }
    }
};

struct Server {
    sf::UdpSocket sock;
    u16 port;

    GameState game_state;
    std::vector<Event> events;
    std::vector<Event> all_events;
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

            Peers::PeerRemovalCallback on_peer_removal = [this](const PeerInfo& info){
                auto removal = PlayerRemove {
                    .player_id = info.id,
                };
                auto event = Event {
                    .tick = tick,
                    .content = removal,
                };
                std::cout << "removing player with id = " << info.id << " tick " << tick << " last " << info.tick_of_last_event << std::endl;

                events.push_back(event);
            };
            peers.remove_inactive_peers(tick, on_peer_removal);

            // apply events received from clients
            apply_events(events);
            auto number_of_events_received = events.size();

            logic();

            // apply events generated by server
            std::span e = events;
            apply_events(e.subspan(number_of_events_received));

            flush_events();

            auto elapsed = clock.restart().asSeconds();
            auto toSleep = std::max(TICK_TIME - elapsed, 0.f);
            sf::sleep(sf::seconds(toSleep));

            tick++;
        }
    }

    void handle_packet(sf::Packet& pkt, SockAddr addr) {
        //std::cout <<
            //"received packet from " << addr.first << ":" << addr.second << "\n" <<
            //"    length = " << pkt.getDataSize() << std::endl;

        Event event;
        pkt >> event;

        std::visit([&](auto &&arg) -> void {
                handle_event(arg, addr);
        }, event.content);
    }

    void handle_event(Hello h, SockAddr addr) {
        std::cout << "received hello message\n" <<
            "    username = " << h.username << "\n" <<
            "    addr = " << addr.first << ":" << addr.second << std::endl;

        auto id = peers.create_peer(addr, h.username, tick);
        peers.keep_peer_alive(tick, id);

        std::cout << "    assigning id = " << id << std::endl;


        auto response = HelloResponse {
            .player_id = id
        };

        auto event = Event {
            .tick = 0,
            .content = response,
        };

        // XDDDDDDDDDDDDDDD
        sf::TcpListener listener;
        listener.listen(addr.second);

        send_event(event, addr);

        sf::TcpSocket ts;
        listener.accept(ts);

        std::cout << "    len of stream = " << all_events.size() << std::endl;
        sf::Packet pack;
        for (auto &&old_event : all_events) {
            pack.clear();
            pack << old_event;
            ts.send(pack);
        }
        ts.disconnect();
        listener.close();
    }

    void handle_event(HelloResponse, SockAddr addr) {
        std::cout << "received hello response\n" <<
            "    addr = " << addr.first << ":" << addr.second << std::endl;
    }

    void handle_event(PlayerPos pp, SockAddr) {
        auto event = Event {
            .tick = tick,
            .content = pp,
        };

        if (peers.id_to_addr.find(pp.player_id) == peers.id_to_addr.end())
            return;
        peers.keep_peer_alive(tick, pp.player_id);

        events.push_back(event);
    }

    void handle_event(BulletShot bs, SockAddr) {
        auto event = Event {
            .tick = tick,
            .content = bs,
        };

        events.push_back(event);
    }

    void handle_event(BulletPos, SockAddr) {
    }

    void handle_event(BulletRemove, SockAddr ) {
    }

    void handle_event(PlayerRemove, SockAddr ) {
    }

    void handle_event(PlayerStatsChange change, SockAddr ) {
        auto event = Event {
            .tick = tick,
            .content = change,
        };

        events.push_back(event);
    }

    void send_event(Event event, SockAddr to) {
        sf::Packet pkt;
        pkt << event;
        sock.send(pkt, to.first, to.second);
    }

    void flush_events() {
        for (auto &peer : peers.peers) {
            auto peer_id = peer.second.id;

            for (auto &event : events) {
                auto owner_id = get_event_owner_id(event);
                peers.keep_peer_alive(tick, owner_id);
                if (owner_id != peer_id) {
                    send_event(event, peer.first);
                }
            }
        }

        for (auto &event : events) {
            all_events.push_back(event);
        }

        events.clear();
    }

    u64 get_event_owner_id(const Event& event) {
        if (std::holds_alternative<PlayerPos>(event.content)) {
            return std::get<PlayerPos>(event.content).player_id;
        }
        return 0;
    }

    void apply_events(const std::span<Event>& e) {
        game_state.apply_events(e);
    }

    void logic() {
        for (auto &&[id, bullet] : game_state.bullets) {
            collide_with_players(bullet, id);
            //std::cout << "bullet id = " << id << std::endl;
            auto new_position = bullet.pos + bullet.direction * BULLET_SPEED;

            if (std::abs(new_position.x) + std::abs(new_position.y) > 2048) {
                auto bullet_remove = BulletRemove {
                    .bullet_id = id,
                };

                events.push_back(Event{
                    .tick = tick,
                    .content = bullet_remove,
                });
            } else {
                auto bullet_pos = BulletPos {
                    .bullet_id = id,
                    .pos = new_position,
                };

                events.push_back(Event {
                    .tick = tick,
                    .content = bullet_pos,
                });
            }
        }
    }

    void collide_with_players(const Bullet& b, u64 bullet_id) {
        for (auto &&[id, player] : game_state.players) {
            if (player.dead)
                continue;

            auto player_pos = player.pos;
            auto bullet_pos = b.pos;

            auto diff = player_pos - bullet_pos;
            auto distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            if (distance < 15.f) {
                auto state_change = PlayerStatsChange {
                    .player_id = id,
                    .dead = true,
                    .score = player.score,
                    .name = player.name,
                };
                push_event(state_change);

                auto bullet_remove = BulletRemove {
                    .bullet_id = bullet_id,
                };
                push_event(bullet_remove);

                increase_score(b.owner_id);
            }
        }
    }

    void increase_score(u64 player_id) {
        if (game_state.players.count(player_id) == 0)
            return;

        Player& player = game_state.players[player_id];

        auto state_change = PlayerStatsChange {
            .player_id = player_id,
            .dead = player.dead,
            .score = player.score + 1,
            .name = player.name,
        };

        push_event(state_change);
    }

    template<typename E>
    void push_event(E event) {
        events.push_back(Event{
            .tick = tick,
            .content = event,
        });
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

