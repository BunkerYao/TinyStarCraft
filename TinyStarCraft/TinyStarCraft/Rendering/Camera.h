#pragma once

#include "Utilities/Size2.h"

namespace TinyStarCraft
{ 

/**
 *	Isometric camera class
 */
class Camera
{
public:
    /**	Constructor */
    Camera(const Size2f& viewportSize, const D3DXVECTOR3& position);

    /** Set the world space position. */
    void setPosition(const D3DXVECTOR3& position);

    /** Get the world space position */
    const D3DXVECTOR3& getPosition() const { return mPosition; }

    /** Get the view matrix. */
    const D3DXMATRIX& getViewMatrix();

    /** Get the projection matrix. */
    const D3DXMATRIX& getProjMatrix();

private:
    /* Update the view matrix form current camera position. */
    void _updateViewMatrix();

    /** Update the projection matrix from current viewport size. */
    void _updateProjMatrix();

private:
    Size2f mViewportSize;
    D3DXVECTOR3 mPosition;
    D3DXMATRIX mViewMatrix;
    D3DXMATRIX mProjMatrix;
    bool mViewMatrixNeedsUpdate;
    bool mProjMatrixNeedsUpdate;
};

};



