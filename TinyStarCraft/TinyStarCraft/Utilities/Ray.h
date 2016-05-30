#pragma once

namespace TinyStarCraft
{

class AABB;

class Ray
{
public:
    Ray() = default;

    Ray(const D3DXVECTOR3& origin, const D3DXVECTOR3& direction)
        : mOrigin(origin), mDirection(direction)
    {}

    /** Get the origin. */
    const D3DXVECTOR3& getOrigin() const { return mOrigin; }

    /** Get the origin. */
    D3DXVECTOR3& getOrigin() { return mOrigin; }

    /** Modify the origin. */
    void setOrigin(const D3DXVECTOR3& origin) { mOrigin = origin; }

    /** Get the direction. */
    const D3DXVECTOR3& getDirection() const { return mDirection; }

    /** Get the direction. */
    D3DXVECTOR3& getDirection() { return mDirection; }

    /** Modify the direction. */
    void setDirection(const D3DXVECTOR3& direction) { mDirection = direction; }

    /** Normalize the direction vector */
    void normalize() { ::D3DXVec3Normalize(&mDirection, &mDirection); }

    /**	Get a point on this ray. */
    D3DXVECTOR3 getPointOnRay(float t) const
    {
        return mDirection * t + mOrigin;
    }

    /**
      	Test if this ray has intersection with an AABB.
     */
    bool intersectAABB(const AABB& aabb) const;

    /**
      	Test if this ray has intersection with an AABB and return the intersection positions.
     */
    bool intersectAABB(const AABB& aabb, float t[2]) const;

private:
    
    bool _intersectAABBImpl(const AABB& aabb, float t[2]) const;

private:
    D3DXVECTOR3 mOrigin;
    D3DXVECTOR3 mDirection;
};

};
