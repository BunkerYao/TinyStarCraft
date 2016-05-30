#pragma once

#include "ResourceManager.h"
#include "Utilities/Size2.h"

namespace TinyStarCraft
{

class Texture;

/**
 	Manages all texture resources creation and destroy.
@remarks
    Textures created by this texture manager could be retrieved by a resource name.
    Textures created as render targets will be recreated by the texture manager when device
    is lost.
 */
class TextureManager : public ResourceManager
{
public:
    /**
    	Resource name of the default texture
    @remarks
        Default texture is a empty texture created by the texture manager when initialize.
     */
    static constexpr char DEFAULT_TEXTURE_RESOURCE_NAME[12] = "__default__";

public:
    /** Constructor */
    explicit TextureManager(IDirect3DDevice9* D3DDevice);

    /** Initialize the texture manager */
    bool initialize();

    /**
      	Create an empty texture.
    @param size
        Size of the texture in pixels.
    @param mipLevels
        Number of Mipmap levels.
    @param format
        Format for the texture.
    @param isRenderTarget
        If the texture is created as a render texture.
     */
    Texture* createTexture(const std::string& resourceName, const Size2<UINT>& size = Size2<UINT>::ZERO(),
        UINT mipLevels = D3DX_DEFAULT, D3DFORMAT format = D3DFMT_UNKNOWN, bool isRenderTarget = false);

    /**
      	Create an texture from image file.
    @param size
        Size of the texture in pixels. If set value to zero or D3DX_DEFAULT, the dimensions are taken
        from the file and rounded up to a power of two. If the device supports non-power of 2 textures 
        and D3DX_DEFAULT_NONPOW2 is specified, the size will not be rounded.
    @param mipLevels
        Number of Mipmap levels.
    @param format
        Format for the texture.
    @param isRenderTarget
        If the texture is created as a render texture.
     */
    Texture* createTextureFromFile(const std::string& resourceName, const std::string& fileName, 
        const Size2<UINT>& size = Size2<UINT>::ZERO(), UINT mipLevels = D3DX_DEFAULT, D3DFORMAT format = D3DFMT_UNKNOWN,
        bool isRenderTarget = false);

    /**
      	Retrieve the Texture with resource name.
    @return
        Returns nullptr if the resource is not exist.
     */
    Texture* getTexture(const std::string& name) const;

    /** Unload all texture created with render target usage. */
    void unloadRenderTargets();

    /** Reload all textures created with render target usage. */
    bool reloadRenderTargets();

private:
    IDirect3DDevice9* mD3DDevice;
};

}

