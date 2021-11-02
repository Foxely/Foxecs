
#pragma once

#include "Math/Vec2.hpp"

struct Transform
{
    Transform(Vec2 pos) : position(pos) {}
    Vec2 position;
};
