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
};
sf::Packet& operator <<(sf::Packet& packet, const PlayerPos& p);
sf::Packet& operator >>(sf::Packet& packet, PlayerPos& p);

struct BulletShot {
    u64 bullet_id;
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

using EventVariant = std::variant<
    PlayerPos,
    BulletShot,
    Hello,
    HelloResponse,
    BulletPos,
    BulletRemove
>;

struct Event {
    u64 tick;
    EventVariant content;
};

sf::Packet& operator <<(sf::Packet& packet, const Event& event);
sf::Packet& operator >>(sf::Packet& packet, Event& event);

void test_event_serialization();
