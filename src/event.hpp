#pragma once

#include <SFML/Network.hpp>
#include <variant>

#include "types.hpp"

struct PlayerPos {
    u64 player_id;
    vec2f pos;
};

sf::Packet& operator <<(sf::Packet& packet, const PlayerPos& p);
sf::Packet& operator >>(sf::Packet& packet, PlayerPos& p);

struct BulletShot {
    vec2f pos;
    vec2f direction;
};

sf::Packet& operator <<(sf::Packet& packet, const BulletShot& b);
sf::Packet& operator >>(sf::Packet& packet, BulletShot& b);

using EventVariant = std::variant<PlayerPos, BulletShot>;

struct Event {
    u64 tick;
    EventVariant content;
};

sf::Packet& operator <<(sf::Packet& packet, const Event& event);
sf::Packet& operator >>(sf::Packet& packet, Event& event);

void test_event_serialization();
