#include "Precompiled.h"
#include "Camera.h"
#include "Utilities/AABB.h"

namespace TinyStarCraft
{

Camera::Camera(const Size2f& viewportSize, const D3DXVECTOR3& position)
    : mViewportSize(viewportSize),
      mPosition(position), 
      mViewMatrixNeedsUpdate(true),
      mProjMatrixNeedsUpdate(true),
      mViewFrustumNeedsUpdate(true)
{
}

void Camera::setPosition(const D3DXVECTOR3& position)
{
    mPosition = position;
    mViewMatrixNeedsUpdate = true;
    mViewFrustumNeedsUpdate = true;
}

void Camera::setViewportSize(const Size2f& size)
{
    mViewportSize = size;
    mProjMatrixNeedsUpdate = true;
    mViewFrustumNeedsUpdate = true;
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

D3DXMATRIX Camera::getTransformMatrix()
{
    if (mViewMatrixNeedsUpdate)
        _updateViewMatrix();

    // Transform matrix is the inverse of the view matrix.
    D3DXMATRIX transform;
    ::D3DXMatrixInverse(&transform, NULL, &mViewMatrix);
    return transform;
}

const ViewFrustum& Camera::getViewFrustum()
{
    if (mViewFrustumNeedsUpdate)
        _updateViewFrustum();

    return mViewFrustum;
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
    ::D3DXMatrixOrthoLH(&mProjMatrix, mViewportSize.x, mViewportSize.y, 0.0f, 10000.0f);

    mProjMatrixNeedsUpdate = false;
}

void Camera::_updateViewFrustum()
{
    // Left-top, right-top and left-bottom corner position of the viewport rectangle
    // in view space.
    D3DXVECTOR4 viewPlaneVertices[] =
    {
        { -mViewportSize.x * 0.5f,  mViewportSize.y * 0.5f, 0.0f, 1.0f },
        {  mViewportSize.x * 0.5f,  mViewportSize.y * 0.5f, 0.0f, 1.0f },
        { -mViewportSize.x * 0.5f, -mViewportSize.y * 0.5f, 0.0f, 1.0f },
    };
    // Transform to world space.
    D3DXMATRIX cameraTransformMatrix = getTransformMatrix();
    ::D3DXVec4TransformArray(viewPlaneVertices, sizeof(D3DXVECTOR4), viewPlaneVertices, sizeof(D3DXVECTOR4), &cameraTransformMatrix, 3);

    D3DXPLANE plane;
    D3DXVECTOR4 vecNormal;
    // Top Plane
    vecNormal = viewPlaneVertices[2] - viewPlaneVertices[0];
    ::D3DXVec4Normalize(&vecNormal, &vecNormal);
    ::D3DXPlaneFromPointNormal(&plane, (D3DXVECTOR3*)&viewPlaneVertices[0], (D3DXVECTOR3*)&vecNormal);
    mViewFrustum.setPlane(0, plane);

    // Left Plane
    vecNormal = viewPlaneVertices[1] - viewPlaneVertices[0];
    ::D3DXVec4Normalize(&vecNormal, &vecNormal);
    ::D3DXPlaneFromPointNormal(&plane, (D3DXVECTOR3*)&viewPlaneVertices[0], (D3DXVECTOR3*)&vecNormal);
    mViewFrustum.setPlane(1, plane);

    // Right Plane
    vecNormal = viewPlaneVertices[0] - viewPlaneVertices[1];
    ::D3DXVec4Normalize(&vecNormal, &vecNormal);
    ::D3DXPlaneFromPointNormal(&plane, (D3DXVECTOR3*)&viewPlaneVertices[1], (D3DXVECTOR3*)&vecNormal);
    mViewFrustum.setPlane(2, plane);

    // Bottom Plane
    vecNormal = viewPlaneVertices[0] - viewPlaneVertices[2];
    ::D3DXVec4Normalize(&vecNormal, &vecNormal);
    ::D3DXPlaneFromPointNormal(&plane, (D3DXVECTOR3*)&viewPlaneVertices[2], (D3DXVECTOR3*)&vecNormal);
    mViewFrustum.setPlane(3, plane);
}

ViewFrustum::ViewFrustum(const D3DXPLANE planes[4])
{
    for (int i = 0; i < 4; ++i)
        mPlanes[i] = planes[i];
}

int ViewFrustum::hasIntersection(const AABB& aabb) const
{
    const D3DXVECTOR3& aabbMin = aabb.getMin();
    const D3DXVECTOR3& aabbMax = aabb.getMax();

    int ret = AABB_INSIDE;

    // Calculate min & max points for each face and check their position.
    D3DXVECTOR4 max(0.0f, 0.0f, 0.0f, 1.0f), min(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < 4; ++i) {
        // X-Axis
        if (mPlanes[i].a > 0.0f) {
            max.x = aabbMax.x;
            min.x = aabbMin.x;
        }
        else {
            max.x = aabbMin.x;
            min.x = aabbMax.x;
        }

        // Y-Axis
        if (mPlanes[i].b > 0.0f) {
            max.y = aabbMax.y;
            min.y = aabbMin.y;
        }
        else {
            max.y = aabbMin.y;
            min.y = aabbMax.y;
        }

        // Z-Axis
        if (mPlanes[i].c > 0.0f) {
            max.z = aabbMax.z;
            min.z = aabbMin.z;
        }
        else {
            max.z = aabbMin.z;
            min.z = aabbMax.z;
        }

        if (::D3DXPlaneDot(&mPlanes[i], &max) < 0.0f)
            return AABB_OUTSIDE;

        if (::D3DXPlaneDot(&mPlanes[i], &min) < 0.0f)
            ret = AABB_INTERSECT;
    }

    return ret;
}

}
