#include "Precompiled.h"
#include "Ray.h"
#include "AABB.h"

namespace TinyStarCraft
{

bool Ray::intersectAABB(const AABB& aabb) const
{
    float t[2];
    return _intersectAABBImpl(aabb, t);
}

bool Ray::intersectAABB(const AABB& aabb, float t[2]) const
{
    return _intersectAABBImpl(aabb, t);
}

bool Ray::_intersectAABBImpl(const AABB& aabb, float t[2]) const
{
    float tyMin, tyMax, tzMin, tzMax;
    D3DXVECTOR3 invDir(1.0f / mDirection.x, 1.0f / mDirection.y, 1.0f / mDirection.z);

    if (invDir.x > 0.0f) {
        t[0] = (aabb.getMin().x - mOrigin.x) * invDir.x;
        t[1] = (aabb.getMax().x - mOrigin.x) * invDir.x;
    }
    else {
        t[0] = (aabb.getMax().x - mOrigin.x) * invDir.x;
        t[1] = (aabb.getMin().x - mOrigin.x) * invDir.x;
    }

    if (invDir.y > 0.0f) {
        tyMin = (aabb.getMin().y - mOrigin.y) * invDir.y;
        tyMax = (aabb.getMax().y - mOrigin.y) * invDir.y;
    }
    else {
        tyMin = (aabb.getMax().y - mOrigin.y) * invDir.y;
        tyMax = (aabb.getMin().y - mOrigin.y) * invDir.y;
    }

    if (t[0] > tyMax || t[1] < tyMin)
        return false;

    if (tyMin > t[0])
        t[0] = tyMin;
    if (tyMax < t[1])
        t[1] = tyMax;

    if (invDir.z > 0.0f) {
        tzMin = (aabb.getMin().z - mOrigin.z) * invDir.z;
        tzMax = (aabb.getMax().z - mOrigin.z) * invDir.z;
    }
    else {
        tzMin = (aabb.getMax().z - mOrigin.z) * invDir.z;
        tzMax = (aabb.getMin().z - mOrigin.z) * invDir.z;
    }

    if (t[0] > tzMax || t[1] < tzMin)
        return false;

    if (tzMin > t[0])
        t[0] = tzMin;
    if (tzMax < t[1])
        t[1] = tzMax;

    return true;
}

}