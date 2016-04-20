#pragma once

#include "Vector2.h"

namespace TinyStarCraft
{
    template <typename T>
    using Size2 = Vector2<T>;

    typedef Size2<int> Size2d;
    typedef Size2<float> Size2f;
};