#include "Precompiled.h"
#include "TextureManager.h"
#include "Texture.h"
#include "Rendering/RenderSystem.h"
#include "Utilities/Assert.h"
#include "Utilities/Logging.h"

namespace TinyStarCraft
{

TextureManager::TextureManager(IDirect3DDevice9* D3DDevice) 
    : mD3DDevice(D3DDevice)
{
}

bool TextureManager::initialize()
{
    // Create the default texture.
    if (!createTexture(DEFAULT_TEXTURE_RESOURCE_NAME))
    {
        TINYSC_LOGLINE_ERR("Failed to create the default texture.");
        return true;
    }
    
    return true;
}

Texture* TextureManager::createTexture(const std::string& resourceName, const Size2<UINT>& size /*= Size2<UINT>::ZERO()*/,
    UINT mipLevels /*= D3DX_DEFAULT*/, D3DFORMAT format /*= D3DFMT_UNKNOWN*/, bool isRenderTarget /*= false*/)
{
    if (getResource(resourceName)) 
    {
        TINYSC_LOGLINE_ERR("Resource name is in use.");
        return nullptr;
    }

    Texture* texture = new Texture(this, resourceName);
    if (!texture->create(mD3DDevice, size, mipLevels, format, isRenderTarget)) 
    {
        TINYSC_LOGLINE_ERR("Failed to create texture.");
        delete texture;
        return nullptr;
    }

    addResource(texture);

    return texture;
}

Texture* TextureManager::createTextureFromFile(const std::string& resourceName, const std::string& fileName,
    const Size2<UINT>& size /*= Size2<UINT>::ZERO()*/, UINT mipLevels /*= D3DX_DEFAULT*/, D3DFORMAT format /*= D3DFMT_UNKNOWN*/,
    bool isRenderTarget /*= false*/)
{
    if (getResource(resourceName)) 
    {
        TINYSC_LOGLINE_ERR("Resource name is in use.");
        return nullptr;
    }

    Texture* texture = new Texture(this, resourceName);
    if (!texture->createFromFile(mD3DDevice, fileName, size, mipLevels, format, isRenderTarget)) 
    {
        TINYSC_LOGLINE_ERR("Failed to create texture from file %s.", fileName.c_str());
        delete texture;
        return nullptr;
    }

    addResource(texture);

    return texture;
}

Texture* TextureManager::getTexture(const std::string& name) const
{
    return static_cast<Texture*>(getResource(name));
}

void TextureManager::unloadRenderTargets()
{
    // Unload all render targets.
    std::vector<std::string> resourceNames = getResourceNames();
    for (auto& it : resourceNames) 
    {
        Texture* texture = getTexture(it);
        if (texture->isRenderTarget()) 
        {
            texture->unload();
        }
    }
}

bool TextureManager::reloadRenderTargets()
{
    bool result = true;

    // Reload all render targets
    std::vector<std::string> resourceNames = getResourceNames();
    for (auto& it : resourceNames) 
    {
        Texture* texture = getTexture(it);
        if (texture->isRenderTarget()) 
        {
            if (!texture->reload(mD3DDevice)) 
            {
                TINYSC_ASSERT(false, "A render target can't be reloaded.");
                TINYSC_LOGLINE_ERR("Render target %s reload failed.", texture->getName().c_str());
                result = false;
            }
        }
    }

    return result;
}

}



