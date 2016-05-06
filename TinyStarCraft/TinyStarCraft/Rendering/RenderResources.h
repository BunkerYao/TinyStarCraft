#pragma once

#include "Utilities/Assert.h"
#include "Utilities/Size2.h"

namespace TinyStarCraft
{

/**
 *	A class template for warpping various D3D9 resources such as
 *  textures, meshes etc.
 *
 *  @paramT T Resource type, could be IDirect3DTexture9*, IDirect3DVertexBuffer* etc.
 */
template <typename T>
class D3D9ResourceWrapper
{
    friend class RenderSystem;

public:
    /**
     *  Constructor
     *	Wrap a resource.
     */
    explicit D3D9ResourceWrapper(T resource)
        : mResource(resource)
    {
    }

    /**
     *	Destructor. The resource will be released.
     */
    virtual ~D3D9ResourceWrapper()
    {
        _releaseImpl();
    }

    D3D9ResourceWrapper(const D3D9ResourceWrapper<T>&) = delete;
    D3D9ResourceWrapper& operator=(const D3D9ResourceWrapper<T>&) = delete;

    /**
     *	Get the underlying D3D resource object.
     */
    T get() const { return mResource; }

private:
    /** Release the underlying resource object */
    void _releaseImpl()
    {
        ULONG number = mResource->Release();
        TINYSC_ASSERT(number == 0, "Release unsuccessful.");
        mResource = NULL;
    }

private:
    T mResource;
};


/**
 *	Wrapper class for IDirect3DTexture9 interface.
 */
class Texture : public D3D9ResourceWrapper<IDirect3DTexture9*>
{
public:
    explicit Texture(IDirect3DTexture9* texture)
        : D3D9ResourceWrapper<IDirect3DTexture9*>(texture)
    {
        TINYSC_ASSERT(texture, "Texture is null.");
    }
};


/**
 *	Wrapper class for ID3DXMesh interface.
 */
class Mesh : public D3D9ResourceWrapper<ID3DXMesh*>
{
public:
    explicit Mesh(ID3DXMesh* mesh)
        : D3D9ResourceWrapper<ID3DXMesh*>(mesh)
    {
        TINYSC_ASSERT(mesh, "Mesh is null.");
    }
};

};
