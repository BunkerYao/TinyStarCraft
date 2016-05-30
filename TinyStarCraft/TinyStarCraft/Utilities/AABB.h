#pragma once

#include "Math.h"

namespace TinyStarCraft
{

/**
 	Axis-Aligned Bounding Box
 */
class AABB
{
public:
    /** Combine two AABB together to make a counpound AABB */
    static AABB compound(const AABB& a, const AABB& b)
    {
        AABB aabb;

        aabb.getMax().x = Math::max<float>(a.getMax().x, b.getMax().x);
        aabb.getMax().y = Math::max<float>(a.getMax().y, b.getMax().y);
        aabb.getMax().z = Math::max<float>(a.getMax().z, b.getMax().z);

        aabb.getMin().x = Math::min<float>(a.getMin().x, b.getMin().x);
        aabb.getMin().y = Math::min<float>(a.getMin().y, b.getMin().y);
        aabb.getMin().z = Math::min<float>(a.getMin().z, b.getMin().z);

        return aabb;
    }

public:
    /** Constructor */
    AABB() = default;

    /** Constructor */
    AABB(const D3DXVECTOR3& min, const D3DXVECTOR3& max)
        : mMin(min), mMax(max)
    {}

    const D3DXVECTOR3& getMin() const { return mMin; }

    D3DXVECTOR3& getMin() { return mMin; }

    void setMin(const D3DXVECTOR3& val) { mMin = val; }

    const D3DXVECTOR3& getMax() const { return mMax; }

    D3DXVECTOR3& getMax() { return mMax; }

    void setMax(const D3DXVECTOR3& val) { mMax = val; }

private:
    D3DXVECTOR3 mMin, mMax;
};

};
