#include <cassert>
#include <iostream>

#include "event.hpp"

sf::Packet& operator <<(sf::Packet& packet, const vec2f& v) {
    return packet << v.x << v.y;
}
sf::Packet& operator >> (sf::Packet& packet, vec2f& v) {
    return packet >> v.x >> v.y;
}

sf::Packet& operator <<(sf::Packet& packet, const Hello& p) {
    return packet << p.username;
}
sf::Packet& operator >>(sf::Packet& packet, Hello& p) {
    return packet >> p.username;
}

sf::Packet& operator <<(sf::Packet& packet, const HelloResponse& p) {
    return packet << p.player_id;
}
sf::Packet& operator >>(sf::Packet& packet, HelloResponse& p) {
    return packet >> p.player_id;
}

sf::Packet& operator <<(sf::Packet& packet, const PlayerPos& p) {
    return packet << p.player_id << p.pos << p.look_dir;
}
sf::Packet& operator >>(sf::Packet& packet, PlayerPos& p) {
    return packet >> p.player_id >> p.pos >> p.look_dir;
}

sf::Packet& operator <<(sf::Packet& packet, const BulletShot& b) {
    return packet << b.pos << b.direction << b.bullet_id << b.owner_id;
}
sf::Packet& operator >>(sf::Packet& packet, BulletShot& b) {
    return packet >> b.pos >> b.direction >> b.bullet_id >> b.owner_id;
}

sf::Packet& operator <<(sf::Packet& packet, const BulletPos& b) {
    return packet << b.pos << b.bullet_id;
}
sf::Packet& operator >>(sf::Packet& packet, BulletPos& b) {
    return packet >> b.pos >> b.bullet_id;
}

sf::Packet& operator <<(sf::Packet& packet, const BulletRemove& b) {
    return packet << b.bullet_id;
}
sf::Packet& operator >>(sf::Packet& packet, BulletRemove& b) {
    return packet >> b.bullet_id;
}

sf::Packet& operator <<(sf::Packet& packet, const PlayerRemove& b) {
    return packet << b.player_id;
}
sf::Packet& operator >>(sf::Packet& packet, PlayerRemove& b) {
    return packet >> b.player_id;
}

sf::Packet& operator <<(sf::Packet& packet, const PlayerStatsChange& b) {
    return packet << b.player_id << b.dead << b.score << b.name;
}
sf::Packet& operator >>(sf::Packet& packet, PlayerStatsChange& b) {
    return packet >> b.player_id >> b.dead >> b.score >> b.name;
}

sf::Packet& operator <<(sf::Packet& packet, const BoxAdded& b) {
    return packet << b.box_id << b.pos << b.size;
}
sf::Packet& operator >>(sf::Packet& packet, BoxAdded& b) {
    return packet >> b.box_id >> b.pos >> b.size;
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
        case 2: {
            Hello h;
            packet >> h;
            event.content = h;
            break;
        }
        case 3: {
            HelloResponse h;
            packet >> h;
            event.content = h;
            break;
        }
        case 4: {
            BulletPos h;
            packet >> h;
            event.content = h;
            break;
        }
        case 5: {
            BulletRemove h;
            packet >> h;
            event.content = h;
            break;
        }
        case 6: {
            PlayerRemove h;
            packet >> h;
            event.content = h;
            break;
        }
        case 7: {
            PlayerStatsChange h;
            packet >> h;
            event.content = h;
            break;
        }
        case 8: {
            BoxAdded b;
            packet >> b;
            event.content = b;
            break;
        }
    }

    return packet;
}

static void test_player_pos_event() {
    PlayerPos pos;
    pos.player_id = 456;
    pos.pos = {7, 8};
    pos.look_dir = {1, 2};

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
    assert(std::get<PlayerPos>(deser.content).look_dir == pos.look_dir);
}

static void test_hello_event() {
    Hello h;
    h.username = "hej";

    Event e;
    e.tick = 123;
    e.content = h;

    // serialize
    sf::Packet pkt;
    pkt << e;

    // deserialize
    Event deser;
    pkt >> deser;

    // verify
    assert(deser.tick == 123);
    assert(std::get<Hello>(deser.content).username == "hej");
}

static void test_bullet_remove_event() {
    BulletRemove h;
    h.bullet_id = 1234;

    Event e;
    e.tick = 123;
    e.content = h;

    // serialize
    sf::Packet pkt;
    pkt << e;

    // deserialize
    Event deser;
    pkt >> deser;

    // verify
    assert(deser.tick == 123);
    assert(std::get<BulletRemove>(deser.content).bullet_id == 1234);
}

static void test_player_remove_event() {
    PlayerRemove h;
    h.player_id = 1234;

    Event e;
    e.tick = 123;
    e.content = h;

    // serialize
    sf::Packet pkt;
    pkt << e;

    // deserialize
    Event deser;
    pkt >> deser;

    // verify
    assert(deser.tick == 123);
    assert(std::get<PlayerRemove>(deser.content).player_id == 1234);
}

static void test_player_stats_change_event() {
    PlayerStatsChange h;
    h.player_id = 1234;
    h.dead = false;
    h.score = -69;
    h.name = "e";

    Event e;
    e.tick = 123;
    e.content = h;

    // serialize
    sf::Packet pkt;
    pkt << e;

    // deserialize
    Event deser;
    pkt >> deser;

    // verify
    assert(deser.tick == 123);
    assert(std::get<PlayerStatsChange>(deser.content).player_id == 1234);
    assert(std::get<PlayerStatsChange>(deser.content).dead == false);
    assert(std::get<PlayerStatsChange>(deser.content).score == -69);
    assert(std::get<PlayerStatsChange>(deser.content).name == "e");
}

static void test_box_added_event() {
    BoxAdded box_added;
    box_added.box_id = 456;
    box_added.pos = vec2f(6, 7);
    box_added.size = vec2f(1, 8);

    Event e;
    e.tick = 5454;
    e.content = box_added;

    // serialize
    sf::Packet pkt;
    pkt << e;

    // deserialize
    Event deser;
    pkt >> deser;

    assert(deser.tick == 5454);
    assert(std::get<BoxAdded>(deser.content).box_id == 456);
    assert(std::get<BoxAdded>(deser.content).pos == vec2f(6, 7));
    assert(std::get<BoxAdded>(deser.content).size == vec2f(1, 8));
}

void test_event_serialization() {
    std::cout << "testing event [de]serialization" << std::endl;
    test_player_pos_event();
    test_hello_event();
    test_bullet_remove_event();
    test_player_remove_event();
    test_player_stats_change_event();
    test_box_added_event();
    std::cout << "  event [de]serialization ok" << std::endl;
}

