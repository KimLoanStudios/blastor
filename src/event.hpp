#pragma once
#include "types.hpp"
#include <variant>

struct PlayerPos {
    u64 player_id;
    vec2f pos;
};

struct BulletShot {
    vec2f pos;
    vec2f direction;
};

using EventVariant = std::variant<PlayerPos, BulletShot>;

struct Event {
    u64 tick;
    EventVariant content;
};

