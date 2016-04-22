#pragma once

#include "Utilities/Assert.h"
#include "Utilities/Size2.h"

namespace TinyStarCraft
{

/**
 *	Wrapper class for IDirect3DTexture9 interface.
 */
class Texture
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
        : mD3dTexture(texture)
    {
        TINYSC_ASSERT(texture, "Texture is null.");
    }

    /**
     *	Destructor. The texture will be released.
     */
    ~Texture()
    {
        _releaseImpl();
    }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    /**
     *	A Texture object is convertable to a IDirect3DTexture9 object.
     */
    operator IDirect3DTexture9* () const
    {
        return mD3dTexture;
    }

    /**
     *	Override the arrow operator so to make the texture object act like a 
     *  IDirect3DTexture9 object. But don't call Release since the wrapper
     *  will handle it.
     */
    IDirect3DTexture9* operator->() const
    {
        return mD3dTexture;
    }

private:
    /** Release the underlying texture object */
    void _releaseImpl()
    {
        ULONG number = mD3dTexture->Release();
        TINYSC_ASSERT(number == 0, "IDirect3DTexture9::Release unsuccessful.");
    }

private:
    IDirect3DTexture9* mD3dTexture;
};

}

