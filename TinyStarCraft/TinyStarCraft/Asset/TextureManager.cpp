#include "Precompiled.h"
#include "TextureManager.h"
#include "Rendering/RenderSystem.h"

namespace TinyStarCraft
{

Texture* TextureManager::createFromFile(const std::string& resourceName, const std::string& fileName, const Size2<UINT>& size,
    UINT mipLevels, D3DFORMAT format, bool isRenderTarget)
{
    // Check whether the resource name is in use.
    if (getResource(resourceName) != nullptr) {
        printf("Try to create a texture resource with a resource name \"%s\" already in use.\n", resourceName.c_str());
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



