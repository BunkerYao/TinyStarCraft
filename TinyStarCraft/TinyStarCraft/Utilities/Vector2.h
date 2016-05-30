#pragma once

namespace TinyStarCraft
{

/**
 *	A class template represents 2 dimensional vector or point.
 */
template <typename T>
class Vector2
{
public:
    Vector2() = default;

    Vector2(T x, T y)
        : x(x), y(y)
    {}

    Vector2<T> operator+(const Vector2<T>& rhs) const
    {
        return Vector2<T>(x + rhs.x, y + rhs.y);
    }

    const Vector2<T>& operator+=(const Vector2& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vector2<T> operator-(const Vector2<T>& rhs) const
    {
        return Vector2<T>(x - rhs.x, y - rhs.y);
    }

    const Vector2<T>& operator-=(const Vector2<T>& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Vector2<T> operator*(T s) const
    {
        return Vector2<T>(x * s, y * s);
    }

    const Vector2<T>& operator*=(T s) const
    {
        x *= s;
        y *= s;
        return *this;
    }

    Vector2<T> operator/(T s) const
    {
        return Vector2<T>(x / s, y / s);
    }

    const Vector2<T>& operator/=(T s) const
    {
        x /= s;
        y /= s;
        return *this;
    }

    template <typename U>
    operator Vector2<U>() const
    {
        return Vector2<U>(U(x), U(y));
    }

public:
    /**
      	Obtain a vector which has all component set to zero.
     */
    static const Vector2<T>& ZERO()
    {
        static const Vector2<T> ZERO(T(0), T(0));
        return ZERO;
    }

    /**
      	Obtain a vector which has all component set to one.
     */
    static const Vector2<T>& ONE()
    {
        static const Vector2<T> ONE(T(1), T(1));
        return ONE;
    }

public:
    T x, y;
};

};