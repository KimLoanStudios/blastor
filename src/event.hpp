#pragma once
#include "types.hpp"
#include <variant>

struct Event {
    u64 tick;
    EventVariant content;
};

struct PlayerPos {
    u64 player_id;
    f32 x;
    f32 y;
};

struct BulletShot {
    f32 x;
    f32 y;
    f32 direction_x;
    f32 direction_y;
};

using EventVariant = std::variant<PlayerPos, BulletShot>;
