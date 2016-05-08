#include "Precompiled.h"
#include "TextureManager.h"
#include "Rendering/RenderSystem.h"
#include "Utilities/Logging.h"

namespace TinyStarCraft
{

Texture* TextureManager::createTextureFromFile(const std::string& resourceName, const std::string& fileName,
    const Size2<UINT>& size /*= Size2<UINT>::ZERO()*/, UINT mipLevels /*= D3DX_DEFAULT*/, D3DFORMAT format /*= D3DFMT_UNKNOWN*/,
    bool isRenderTarget /*= false*/)
{
    // Check whether the resource name is in use.
    if (getResource(resourceName) != nullptr) {
        TINYSC_LOGLINE_ERR("Texture name \"%s\" is already in use.\n", resourceName.c_str());
        return nullptr;
    }

    // Create the texture.
    Texture* texture = mRenderSystem->createTextureFromFile(fileName, size, mipLevels, format, isRenderTarget);
    if (texture == nullptr) {
        return nullptr;
    }

    // Wrap the texture and add to the resource manager.
    TextureResourceContainer* container = new TextureResourceContainer(resourceName, texture, mRenderSystem);
    addResource(container);

    return texture;
}

Texture* TextureManager::getTexture(const std::string& name) const
{
    TextureResourceContainer* container = static_cast<TextureResourceContainer*>(getResource(name));

    if (container) 
        return container->getTexture();
    else 
        return nullptr;
}

TextureManager::TextureResourceContainer::~TextureResourceContainer()
{
    mRenderSystem->destroyTexture(mTexture);
}

}



