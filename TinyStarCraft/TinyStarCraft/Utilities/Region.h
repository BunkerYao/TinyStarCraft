#pragma once

#include "Rect2.h"

namespace TinyStarCraft
{

/**
  	Defines a rectangle area at x-z plane in world space.
 */
class Region
{
public:
    /** Constructor */
    Region() = default;

    /**
        Constructor 
    @param min
        Minimum extend point of the region.
        Y-component is ignored.
    @param max
        Maximum extend point of the region.
        Y-component is ignored
     */
    Region(const D3DXVECTOR3& min, const D3DXVECTOR3& max);

    /**
      	Get the minimum extend point.
        Y component in the returned position is set to 0.
     */
    D3DXVECTOR3 getMin() const
    {
        return D3DXVECTOR3(mRect.getMin().x, 0.0f, mRect.getMin().y);
    }

    /**
      	Get the maximum extend point.
        Y component in the returned position is set to 0.
     */
    D3DXVECTOR3 getMax() const
    {
        return D3DXVECTOR3(mRect.getMax().x, 0.0f, mRect.getMax().y);
    }

    /**
      	Get the size.
     */
    D3DXVECTOR3 getSize() const
    {
        Size2f size = mRect.getSize();
        return D3DXVECTOR3(size.x, 0.0f, size.y);
    }

    /** Clamp the x, z component of the point in the region. */
    D3DXVECTOR3 clamp(const D3DXVECTOR3& point) const
    {
        Point2f result = mRect.clamp(Point2f(point.x, point.z));
        return D3DXVECTOR3(result.x, 0.0f, result.y);
    }

private:
    Rectf mRect;
};

};
