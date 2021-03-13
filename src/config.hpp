#pragma once

#include "types.hpp"

static const u32 TICKS = 60;
static const float TICK_TIME = 1.f/TICKS;

static const u16 SERVER_PORT = 6969;
static const u32 max_inactive_ticks = TICKS * 5;
static const float BULLET_SPEED = TICK_TIME * 200.f;
