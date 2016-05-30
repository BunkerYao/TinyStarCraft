#include "Precompiled.h"
#include "Region.h"

namespace TinyStarCraft
{

Region::Region(const D3DXVECTOR3& min, const D3DXVECTOR3& max)
    : mRect(Point2f(min.x, min.z), Point2f(max.x, max.z))
{
}

}