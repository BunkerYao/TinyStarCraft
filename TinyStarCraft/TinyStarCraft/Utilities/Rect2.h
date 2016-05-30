#pragma once

#include "Math.h"
#include "Point2.h"
#include "Size2.h"

namespace TinyStarCraft
{

/**
 *	A class template represents a rectangle in 2-dimensional space.
 */
template <typename T>
class Rect
{
public:
    Rect() = default;

    Rect(const Point2<T>& min, const Point2<T>& max)
        : mMin(min), mMax(max)
    {}

    template <typename U>
    operator Rect<U>() const
    {
        Rect<U> rect;
        rect.mMin.x = T(mMin.x);
        rect.mMin.y = T(mMin.y);
        rect.mMax.x = T(mMax.x);
        rect.mMax.y = T(mMax.y);
        return rect;
    }

    const Point2<T>& getMin() const { return mMin; }

    Point2<T>& getMin() { return mMin; }

    void setMin(const Point2<T>& val) { mMin = val; }

    const Point2<T>& getMax() const { return mMax; }

    Point2<T>& getMax() { return mMax; }

    void setMax(const Point2<T>& val) { mMax = val; }

    /**
     *	Get the position of the rectangle, i.e., the min point.
     */
    const Point2<T>& getPosition() const { return mMin; }

    /** 
     *  Change the position of the rectangle.
     *  @remarks
     *  This won't change the size of the rectangle.
     */
    void setPosition(const Point2<T>& val)
    {
        WGVector2<T> offset = val - mMin;
        mMin = val;
        mMax = mMax + offset;
    }

    Size2<T> getSize() const
    {
        return mMax - mMin;
    }

    /**
     *	Change the size of the rectangle.
     *  @remarks
     *  The rectangle will maintain its position.
     */
    void setSize(const Size2<T>& val)
    {
        mMax = mMin + val;
    }

    T getWidth() const { return mMax.x - mMin.x; }

    void setWidth(const T& val) { mMax.x = mMin.x + val; }

    T getHeight() const { return mMax.y - mMin.y; }

    void setHeight(const T& val) { mMax.y = mMin.y + val; }

    T getLeft() const { return mMin.x; }

    void setLeft(const T& val) { mMin.x = val; }

    T getRight() const { return mMax.x; }

    void setRight(const T& val) { mMax.x = val; }

    T getTop() const { return mMin.y; }

    void setTop(const T& val) { mMin.y = val; }

    T getBottom() const { return mMax.y; }

    void setBottom(const T& val) { mMax.y = val; }

    /**
      	Get the center point of this rectangle.
     */
    Point2<T> getCenter() const
    {
        return (mMin + mMax) / T(2);
    }

    /**
      	Check if the given point is inside the rectangle.
     */
    bool isPointInside(const Point2<T>& point) const
    {
        return point.x >= mMin.x && point.x < mMax.x && point.y >= mMin.y && point.y < mMax.y;
    }

    /**
      	Clamp a point in the area made by this rectangle.
     */
    Point2<T> clamp(const Point2<T>& point) const
    {
        return Point2<T>(
            Math::clamp<T>(point.x, mMin.x, mMax.x),
            Math::clamp<T>(point.y, mMin.y, mMax.y)
            );
    }

public:
    static Rect<T> makeRect(const Point2<T>& pos, const Size2<T>& size)
    {
        return Rect<T>(pos, pos + size);
    }

    static Rect<T> makeRect(T x, T y, T width, T height)
    {
        return Rect<T>::makeRect(Point2<T>(x, y), Size2<T>(width, height));
    }

    /**
     *	Check if two rectangle are overlapped.
     */
    static bool isOverlapped(const Rect<T>& r1, const Rect<T>& r2)
    {
        return r1.mMin.x < r2.mMax.x && r1.mMax.x > r2.mMin.x &&
            r1.mMin.y < r2.mMax.y && r1.mMax.y > r2.mMin.y;
    }

    /**
     *	Compute the area of the intersection of two rectangle.
     */
    static T intersectionArea(const Rect<T>& r1, const Rect<T>& r2)
    {
        return Math::max<T>(T(0), Math::min<T>(r1.mMax.x, r2.mMax.x) - Math::max<T>(r1.mMin.x, r2.mMin.x)) *
            Math::max<T>(T(0), Math::min<T>(r1.mMax.y, r2.mMax.y) - Math::max<T>(r1.mMin.y, r2.mMin.y));
    }

private:
    Point2<T> mMin;
    Point2<T> mMax;
};

typedef Rect<int> Rectd;
typedef Rect<float> Rectf;

};