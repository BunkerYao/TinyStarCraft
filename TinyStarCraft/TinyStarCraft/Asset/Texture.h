#pragma once

#include "Resource.h"
#include "Utilities/Size2.h"

namespace TinyStarCraft
{

/**
	Texture resource object.
@remarks
    Manages a D3D9 texture resource. Once a texture resource object is being constructed,
    it could create an empty texture or load a texture from file. After a texture is created,
    the texture resource object allows unloading and reloading the texture resource, which is
    useful recreating render targets when handling device lost event.
 */
class Texture : public Resource
{
public:
    /** Constructor */
    Texture(ResourceManager* owner, const std::string& name);

    /** Destructor */
    ~Texture();

    /**
     	Create an empty texture.
    @param D3DDevice
        Which device the texture is created by.
    @param size
        Size of the texture in pixels.
    @param mipLevels
        Number of Mipmap levels.
    @param format
        Pixel format for this texture.
    @param isRenderTarget
        Create this texture as a render target or not.
    @return
        Returns true if texture is created. Returns false if failed.
    @remarks
        A texture can only be created once.
     */
    bool create(IDirect3DDevice9* D3DDevice, const Size2<UINT>& size, UINT mipLevels, D3DFORMAT format, bool isRenderTarget);

    /**
     	Create a texture from loading a image file.
    @param D3DDevice
        Which device the texture is created by.
    @param srcFilename
        Image file name.
    @param size
        Size of the texture in pixels.
    @param mipLevels
        Number of Mipmap levels.
    @param format
        Pixel format for this texture.
    @param isRenderTarget
        Create this texture as a render target or not.
    @return
        Returns true if texture is created. Returns false if failed.
    @remarks
        A texture can only be created once.
     */
    bool createFromFile(IDirect3DDevice9* D3DDevice, const std::string& srcFilename, const Size2<UINT>& size, UINT mipLevels,
        D3DFORMAT format, bool isRenderTarget);

    /**
    	Unload this texture, free its memory.
    @remarks
        The texture must be created before being unloaded. 
     */
    void unload();

    /**
    	Reload a texture being unloaded early.
    @remarks
        Texture must be created and unloaded before being reloaded.
    @return
        Returns true if texture is reloaded. Otherwise returns false.
     */
    bool reload(IDirect3DDevice9* D3DDevice);

    /** Get the D3D texture resource. */
    IDirect3DTexture9* getPointer() const { return mTexture; }

    /** If this texture is a render target. */
    bool isRenderTarget() const { return mIsRenderTarget; }

private:
    // Implementation of creating a texture
    bool _createTextureImpl(IDirect3DDevice9* D3DDevice, const Size2<UINT>& size, UINT mipLevels, D3DFORMAT format, bool isRenderTarget);
    // Implementation of creating a texture from image file.
    bool _createTextureFromFileImpl(IDirect3DDevice9* D3DDevice, const std::string& srcFilename, const Size2<UINT>& size, 
        UINT mipLevels, D3DFORMAT format, bool isRenderTarget);
    // Implementation of unloading a texture
    void _unloadImpl();
    // Implementation of reloading a texture using current settings.
    bool _reloadImpl(IDirect3DDevice9* D3DDevice);

private:
    IDirect3DTexture9* mTexture;
    Size2<UINT> mSize;
    UINT mMipLevels;
    D3DFORMAT mFormat;
    bool mIsRenderTarget;
    std::string mSrcFilename;
    bool mHasTexture;
};

};
