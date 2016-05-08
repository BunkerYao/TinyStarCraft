#include "Precompiled.h"
#include "Camera.h"

namespace TinyStarCraft
{

Camera::Camera(const Size2f& viewportSize, const D3DXVECTOR3& position)
    : mViewportSize(viewportSize), mPosition(position), mViewMatrixNeedsUpdate(true), mProjMatrixNeedsUpdate(true)
{
}

void Camera::setPosition(const D3DXVECTOR3& position)
{
    mPosition = position;
    mViewMatrixNeedsUpdate = true;
}

const D3DXMATRIX& Camera::getViewMatrix()
{
    if (mViewMatrixNeedsUpdate) 
        _updateViewMatrix();

    return mViewMatrix;
}

const D3DXMATRIX& Camera::getProjMatrix()
{
    if (mProjMatrixNeedsUpdate)
        _updateProjMatrix();

    return mProjMatrix;
}

void Camera::_updateViewMatrix()
{
    float lookAtOffset = mPosition.y * sqrtf(1.5f);
    D3DXVECTOR3 lookAt(mPosition.x - lookAtOffset, 0.0f, mPosition.z - lookAtOffset);
    static const D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    ::D3DXMatrixLookAtLH(&mViewMatrix, &mPosition, &lookAt, &up);

    mViewMatrixNeedsUpdate = false;
}

void Camera::_updateProjMatrix()
{
    ::D3DXMatrixOrthoLH(&mProjMatrix, mViewportSize.x, mViewportSize.y, 0.0f, 1000.0f);

    mProjMatrixNeedsUpdate = false;
}

}
