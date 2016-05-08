#pragma once

#include "Utilities/Assert.h"
#include "Utilities/Logging.h"

namespace TinyStarCraft
{

/**
 *	Wrapper class for IDirect3DTexture9 interface.
 */
class Texture
{
    friend class RenderSystem;

public:
    explicit Texture(IDirect3DTexture9* texture)
        : mD3dTexture(texture)
    {
        TINYSC_ASSERT(texture, "Texture is null.");
    }

    ~Texture()
    {
        _releaseImpl();
    }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    IDirect3DTexture9* get() const
    {
        return mD3dTexture;
    }

private:
    /** Release the underlying resource object */
    void _releaseImpl()
    {
        ULONG number = mD3dTexture->Release();
        if (number > 0) {
            TINYSC_LOGLINE_ERR("Texture release unsuccessful %d.", number);
        }
        mD3dTexture = NULL;
    }

private:
    IDirect3DTexture9* mD3dTexture;
};

};
