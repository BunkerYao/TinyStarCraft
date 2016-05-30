#include "Precompiled.h"
#include "IsometricSprite.h"

namespace TinyStarCraft
{

IsometricSprite::IsometricSprite() 
    : mPosition(0.0f, 0.0f, 0.0f),
      mOrigin(0.0f, 0.0f),
      mDimension(Size2f::ZERO()),
      mTextureRect(Point2f::ZERO(), Point2f::ONE()),
      mHeightScale(1.0f),
      mAABBNeedsUpdate(true),
      mMaterial(nullptr)
{
}
//-------------------------------------------------------------------------------------------------
IsometricSprite::IsometricSprite(const D3DXVECTOR3& position, const Point2f& origin, const Size2f& dimension,
    const Rectf& textureRect, float heightScale, Material* material)
    : mPosition(position),
      mOrigin(origin),
      mDimension(dimension),
      mTextureRect(textureRect),
      mHeightScale(heightScale),
      mAABBNeedsUpdate(true),
      mMaterial(material)
{
}
//-------------------------------------------------------------------------------------------------
void IsometricSprite::setPosition(const D3DXVECTOR3& val)
{
    mPosition = val;
    mAABBNeedsUpdate = true;
}
//-------------------------------------------------------------------------------------------------
void IsometricSprite::setOrigin(const Point2f& val)
{
    mOrigin = val;
    mAABBNeedsUpdate = true;
}
//-------------------------------------------------------------------------------------------------
void IsometricSprite::setDimension(const Size2f& val)
{
    mDimension = val;
    mAABBNeedsUpdate = true;
}
//-------------------------------------------------------------------------------------------------
D3DXMATRIX IsometricSprite::calculateWorldTransformMatrix()
{
    D3DXMATRIX worldMatrix;

    ::D3DXMatrixScaling(&worldMatrix, mDimension.x, mDimension.y, 1.0f);

    Vector2<float> localSpaceOffset = mDimension * 0.5f - mOrigin;

    worldMatrix._41 = -localSpaceOffset.x;
    worldMatrix._42 = -localSpaceOffset.y;

    // Matrix that rotate sprite to face to the isometric camera view plane
    static const D3DXMATRIX faceMatrix(
        sqrtf(2.0f) / 2.0f,              0.0f, -sqrtf(2.0f) / 2.0f, 0.0f,
       -sqrtf(2.0f) / 4.0f, sqrt(3.0f) / 2.0f, -sqrtf(2.0f) / 4.0f, 0.0f,
        sqrtf(6.0f) / 4.0f,              0.5f,  sqrtf(6.0f) / 4.0f, 0.0f,
                      0.0f,              0.0f,                0.0f, 1.0f
        );
    
    worldMatrix = worldMatrix * faceMatrix;

    worldMatrix._41 += mPosition.x;
    worldMatrix._42 += mPosition.y;
    worldMatrix._43 += mPosition.z;

    return worldMatrix;
}
//-------------------------------------------------------------------------------------------------
const AABB& IsometricSprite::getAABB()
{
    if (mAABBNeedsUpdate)
    {
        _updateAABB();
        mAABBNeedsUpdate = false;
    }

    return mAABB;
}
//-------------------------------------------------------------------------------------------------
void IsometricSprite::_updateAABB()
{
    // Top-left & bottom-right vertices of the sprite
    static const D3DXVECTOR3 localSpaceVertices[2] = 
    {
        {  0.5f,  0.5f, 0.0f },
        { -0.5f, -0.5f, 0.0f }
    };

    const D3DXMATRIX transformMatrix = calculateWorldTransformMatrix();

    // Transform two vertices to world space
    D3DXVECTOR4 worldSpaceVertices[2];
    ::D3DXVec3TransformArray(worldSpaceVertices, sizeof(D3DXVECTOR4), localSpaceVertices, sizeof(D3DXVECTOR3),
        &transformMatrix, 2);

    mAABB.getMin().x = worldSpaceVertices[1].x;
    mAABB.getMin().y = worldSpaceVertices[1].y;
    mAABB.getMin().z = worldSpaceVertices[0].z;
    mAABB.getMax().x = worldSpaceVertices[0].x;
    mAABB.getMax().y = worldSpaceVertices[0].y;
    mAABB.getMax().z = worldSpaceVertices[1].z;
}

}