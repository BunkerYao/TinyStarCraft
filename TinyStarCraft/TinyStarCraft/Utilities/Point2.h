#pragma once

#include "Vector2.h"

namespace TinyStarCraft
{
    template <typename T>
    using Point2 = Vector2<T>;

    typedef Point2<int> Point2d;
    typedef Point2<float> Point2f;
};