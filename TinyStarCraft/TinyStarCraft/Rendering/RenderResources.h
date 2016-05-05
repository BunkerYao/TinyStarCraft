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
    /**
    *	Options to create a texture.
    */
    struct CreationOptions
    {
        /** Default creation arguments */
        static const CreationOptions& DEFAULT()
        {
            static const CreationOptions defaultArgs(Size2<UINT>::ZERO(), 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED);
            return defaultArgs;
        }

        Size2<UINT> size;
        UINT mipLevels;
        DWORD usage;
        D3DFORMAT format;
        D3DPOOL pool;

        CreationOptions(const Size2<UINT>& size, UINT mipLevels, DWORD usage, D3DFORMAT format, D3DPOOL pool)
            : size(size), mipLevels(mipLevels), usage(usage), format(format), pool(pool)
        {}
    };

public:
    /**
     *	Wrap an IDirect3DTexture object
     */
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
    /**
     *	Options to create a mesh.
     */
    struct CreationOptions
    {
        DWORD numFaces;
        DWORD numVertices;
        DWORD options;
        const D3DVERTEXELEMENT9* declaration;

        CreationOptions(DWORD numFaces, DWORD numVertices, DWORD options, const D3DVERTEXELEMENT9* declaration)
            : numFaces(numFaces), numVertices(numVertices), options(options), declaration(declaration)
        {
        }
    };

public:
    explicit Mesh(ID3DXMesh* mesh)
        : D3D9ResourceWrapper<ID3DXMesh*>(mesh)
    {}
    
};

};
