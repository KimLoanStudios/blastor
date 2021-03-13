#pragma once

#include "types.hpp"
#include <cmath>

inline static f32 veclen(vec2f v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

inline static vec2f normalize(vec2f v) {
    f32 v_len = veclen(v);

    if(v_len != 0) {
        return v / veclen(v);
    }

    return v;
}