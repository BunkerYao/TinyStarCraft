#include "Precompiled.h"
#include "TextureManager.h"
#include "Rendering/RenderSystem.h"

namespace TinyStarCraft
{

Texture* TextureManager::createFromFile(const std::string& resourceName, const std::string& fileName, 
    const Texture::CreationOptions& options /*= Texture::CreationOptions::DEFAULT()*/)
{
    // Check whether the resource name is in use.
    if (getResource(resourceName) != nullptr) {
        printf("Try to create a texture resource with a resource name \"%s\" already in use.\n", resourceName);
        return nullptr;
    }

    // Create the texture use default settings.
    Texture* texture = mRenderSystem->createTextureFromFile(fileName, options);

    if (texture == nullptr) {
        return nullptr;
    }

    // Wrap the texture and add to the resource manager.
    TextureResourceContainer* container = new TextureResourceContainer(resourceName, texture);
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

}



