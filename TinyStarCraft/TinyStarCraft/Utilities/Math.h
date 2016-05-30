#pragma once

namespace TinyStarCraft
{

/**
 *	A class provides variouse math helper functions.
 */
class Math
{
public:
    static const float PI()
    {
        static const float PI = 3.14159f;
        return PI;
    }

public:
    /**
      	Deleted constructor
    @remarks
        For static usage only.
     */
    Math() = delete;

    template <typename T>
    static const T& max(const T& a, const T& b)
    {
        if (a > b)
            return a;
        else
            return b;
    }

    template <typename T>
    static const T& min(const T& a, const T& b)
    {
        if (a < b)
            return a;
        else
            return b;
    }

    template <typename T>
    static const T& clamp(const T& val, const T& lb, const T& rb)
    {
        if (val < lb)
            return lb;
        else if (val > rb)
            return rb;
        else
            return val;
    }

    static float deg2Rad(float val)
    {
        return val * PI() / 180.0f;
    }

    static float rad2Deg(float val)
    {
        return val * 180.0f / PI();
    }

    /**
      	Linear interpolation
     */
    float lerp(float from, float to, float t)
    {
        return from + (to - from) * t;
    }
};

};