#pragma once

#include <SFML/Network.hpp>
#include <variant>

#include "types.hpp"

struct Hello {
    std::string username;
};
sf::Packet& operator <<(sf::Packet& packet, const Hello& p);
sf::Packet& operator >>(sf::Packet& packet, Hello& p);

struct HelloResponse {
    u64 player_id;
};
sf::Packet& operator <<(sf::Packet& packet, const HelloResponse& p);
sf::Packet& operator >>(sf::Packet& packet, HelloResponse& p);

struct PlayerPos {
    u64 player_id;
    vec2f pos;
    vec2f look_dir;
};
sf::Packet& operator <<(sf::Packet& packet, const PlayerPos& p);
sf::Packet& operator >>(sf::Packet& packet, PlayerPos& p);

struct BulletShot {
    u64 bullet_id;
    u64 owner_id;
    vec2f pos;
    vec2f direction;
};
sf::Packet& operator <<(sf::Packet& packet, const BulletShot& b);
sf::Packet& operator >>(sf::Packet& packet, BulletShot& b);

struct BulletPos {
    u64 bullet_id;
    vec2f pos;
};
sf::Packet& operator <<(sf::Packet& packet, const BulletPos& b);
sf::Packet& operator >>(sf::Packet& packet, BulletPos& b);

struct BulletRemove {
    u64 bullet_id;
};
sf::Packet& operator <<(sf::Packet& packet, const BulletRemove& b);
sf::Packet& operator >>(sf::Packet& packet, BulletRemove& b);

struct PlayerRemove {
    u64 player_id;
};
sf::Packet& operator <<(sf::Packet& packet, const PlayerRemove& b);
sf::Packet& operator >>(sf::Packet& packet, PlayerRemove& b);

struct PlayerStatsChange {
    u64 player_id;

    bool dead;
    i32 score;
    std::string name;
};
sf::Packet& operator <<(sf::Packet& packet, const PlayerStatsChange& b);
sf::Packet& operator >>(sf::Packet& packet, PlayerStatsChange& b);

struct BoxAdded {
    u64 box_id;
    vec2f pos;
    vec2f size;
};
sf::Packet& operator <<(sf::Packet& packet, const BoxAdded& b);
sf::Packet& operator >>(sf::Packet& packet, BoxAdded& b);

using EventVariant = std::variant<
    PlayerPos,
    BulletShot,
    Hello,
    HelloResponse,
    BulletPos,
    BulletRemove,
    PlayerRemove,
    PlayerStatsChange,
    BoxAdded
>;

struct Event {
    u64 tick;
    EventVariant content;
};

sf::Packet& operator <<(sf::Packet& packet, const Event& event);
sf::Packet& operator >>(sf::Packet& packet, Event& event);

void test_event_serialization();
