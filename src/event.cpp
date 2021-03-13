#include <cassert>
#include "event.hpp"

sf::Packet& operator <<(sf::Packet& packet, const vec2f& v) {
    return packet << v.x << v.y;
}

sf::Packet& operator >> (sf::Packet& packet, vec2f& v) {
    return packet >> v.x >> v.y;
}

sf::Packet& operator <<(sf::Packet& packet, const PlayerPos& p) {
    return packet << p.player_id << p.pos;
}

sf::Packet& operator >>(sf::Packet& packet, PlayerPos& p) {
    return packet >> p.player_id >> p.pos;
}

sf::Packet& operator <<(sf::Packet& packet, const BulletShot& b) {
    return packet << b.pos << b.direction;
}
sf::Packet& operator >>(sf::Packet& packet, BulletShot& b) {
    return packet >> b.pos >> b.direction;
}

sf::Packet& operator <<(sf::Packet& packet, const Event& event) {
    packet << event.tick;

    u32 index = event.content.index();
    packet << index;
    std::visit([&](auto &&arg) -> void {
            packet << arg;
    }, event.content);

    return packet;
}

sf::Packet& operator >>(sf::Packet& packet, Event& event) {
    packet >> event.tick;

    u32 index;
    packet >> index;
    switch (index) {
        case 0: {
            PlayerPos pos;
            packet >> pos;
            event.content = pos;
            break;
        }
        case 1: {
            BulletShot bs;
            packet >> bs;
            event.content = bs;
            break;
        }
    }

    return packet;
}

void test_event_serialization() {
    PlayerPos pos;
    pos.player_id = 456;
    pos.pos = {7, 8};

    Event e;
    e.tick = 123;
    e.content = pos;

    // serialize
    sf::Packet pkt;
    pkt << e;

    // deserialize
    Event deser;
    pkt >> deser;

    // verify
    assert(deser.tick == 123);
    assert(std::get<PlayerPos>(deser.content).player_id == pos.player_id);
    assert(std::get<PlayerPos>(deser.content).pos == pos.pos);
}
