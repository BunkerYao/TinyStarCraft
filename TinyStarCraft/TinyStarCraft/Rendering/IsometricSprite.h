#pragma once

#include "Utilities/AABB.h"
#include "Utilities/Rect2.h"
#include "Utilities/Size2.h"

namespace TinyStarCraft
{

class Material;

class IsometricSprite
{
public:
    /** Constructor */
    IsometricSprite();

    /** Constructor */
    IsometricSprite(const D3DXVECTOR3& position, const Point2f& origin, const Size2f& dimension,
        const Rectf& textureRect, float heightScale, Material* material);

    const D3DXVECTOR3& getPosition() const { return mPosition; }

    void setPosition(const D3DXVECTOR3& val);

    const Point2f& getOrigin() const { return mOrigin; }

    void setOrigin(const Point2f& val);

    const Size2f& getDimension() const { return mDimension; }

    void setDimension(const Size2f& val);

    const Rectf& getTextureRectangle() const { return mTextureRect; }

    void setTextureRectangle(const Rectf& val) { mTextureRect = val; }

    float getHeightScale() const { return mHeightScale; }

    void setHeightScale(float val) { mHeightScale = val; }

    Material* getMaterial() const { return mMaterial; }

    void setMaterial(Material* val) { mMaterial = val; }

    /** Calculate and return the world transform matrix */
    D3DXMATRIX calculateWorldTransformMatrix();

    /** Get AABB */
    const AABB& getAABB();

private:

    void _updateAABB();

private:
    D3DXVECTOR3 mPosition;
    Point2f mOrigin;

    Size2f mDimension;
    Rectf mTextureRect;
    float mHeightScale;

    AABB mAABB;
    bool mAABBNeedsUpdate;

    Material* mMaterial;
};

};
