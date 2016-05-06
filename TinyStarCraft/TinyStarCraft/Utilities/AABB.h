#pragma once

namespace TinyStarCraft
{

/**
 *	Axis-Aligned Bounding Box
 */
class AABB
{
public:
    AABB() = default;

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
