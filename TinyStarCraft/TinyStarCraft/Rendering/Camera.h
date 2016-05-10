#pragma once

#include "Utilities/Point2.h"
#include "Utilities/Ray.h"
#include "Utilities/Size2.h"

namespace TinyStarCraft
{ 

class AABB;

/**
 *	View frustum of an orthogonal camera
 */
class ViewFrustum
{
public:
    ViewFrustum() = default;

    ViewFrustum(const D3DXPLANE planes[4]);

    /** Get one of the planes in frustum */
    const D3DXPLANE& getPlane(int i) const { return mPlanes[i]; }

    /** Modify one of the planes in frustum */
    void setPlane(int i, const D3DXPLANE& plane) { mPlanes[i] = plane; }

    /** Test whether a point is inside the frustum. */
    bool isPointInside(const D3DXVECTOR4& point) const
    {
        // If the point is in the positive space of all 4 planes, then
        // it is inside the frustum.
        for (int i = 0; i < 4; ++i) {
            if (::D3DXPlaneDot(&mPlanes[i], &point) < 0.0f)
                return false;
        }
        return true;
    }

    static const int AABB_OUTSIDE = 0;
    static const int AABB_INTERSECT = 1;
    static const int AABB_INSIDE = 2;

    /**
     *	Test whether an AABB has intersection with this frustum.
     *
     *  @return
     *  If the AABB is completely outside of the frustum, AABB_OUTSIDE will be returned.
     *  If the AABB is completely inside of the frustum, AABB_INSIDE will be returned.
     *  If the AABB and the frustum has intersection, then AABB_INTERSECTION will be returned.
     */
    int hasIntersection(const AABB& aabb) const;

private:
    D3DXPLANE mPlanes[4];
};


/**
 *	Isometric camera class
 */
class Camera
{
public:
    /**	Constructor */
    Camera(const Size2f& viewportSize, const D3DXVECTOR3& position);

    /** Modify the world space position. */
    void setPosition(const D3DXVECTOR3& position);

    /** Get the world space position */
    const D3DXVECTOR3& getPosition() const { return mPosition; }

    /** Modify the viewport size. */
    void setViewportSize(const Size2f& size);

    /** Get the viewport size. */
    const Size2f& getViewportSize() const { return mViewportSize; }

    /** Get the view matrix. */
    const D3DXMATRIX& getViewMatrix();

    /** Get the projection matrix. */
    const D3DXMATRIX& getProjMatrix();

    /** Get the transform matrix */
    D3DXMATRIX getTransformMatrix();

    /** Get the view frustum. */
    const ViewFrustum& getViewFrustum();

    /* Get a ray going from camera through a screen point. */
    Ray screenPointToRay(const Point2f& point);

private:
    /* Update the view matrix form current camera position. */
    void _updateViewMatrix();

    /** Update the projection matrix from current viewport size. */
    void _updateProjMatrix();

    /** Update the view frustum from current viewport size and camera position */
    void _updateViewFrustum();

private:
    Size2f mViewportSize;
    D3DXVECTOR3 mPosition;
    D3DXMATRIX mViewMatrix;
    D3DXMATRIX mProjMatrix;
    ViewFrustum mViewFrustum;
    bool mViewMatrixNeedsUpdate;
    bool mProjMatrixNeedsUpdate;
    bool mViewFrustumNeedsUpdate;
};





};



