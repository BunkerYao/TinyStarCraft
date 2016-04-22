#pragma once

#include "Texture.h"
#include "Asset/ResourceManager.h"

namespace TinyStarCraft
{

class RenderSystem;
class Texture;

/**
 *	The texture manager is responsible for managing all texture resources
 *  in the life time of the program.
 */
class TextureManager : public ResourceManager
{
private:

    class TextureResourceContainer : public Resource
    {
    public:
        explicit TextureResourceContainer(const std::string& name, Texture* texture)
            : Resource(name),
              mTexture(texture)
        {}

        ~TextureResourceContainer()
        {
            delete mTexture;
        }

        Texture* getTexture() const { return mTexture; }
    private:
        Texture* mTexture;
    };

public:
    
    explicit TextureManager(RenderSystem* renderSystem)
        : mRenderSystem(renderSystem)
    {}

    /**
     *	Create a texture from file.
     *
     *  @param resourceName
     *  A unique name for the new texture resource.
     *
     *  @param filename
     *  The image file name.
     *
     *  @param options
     *  Options to create the texture.
     *
     *  @return
     *  Returns the created texture object. Return null if error happened.
     */
    Texture* createFromFile(const std::string& resourceName, const std::string& fileName, 
        const Texture::CreationOptions& options = Texture::CreationOptions::DEFAULT());

    /**
     *	Get the Texture by resource name.
     *
     *  @return
     *  Returns null if the resource is not exist.
     */
    Texture* getTexture(const std::string& name) const;

private:
    RenderSystem* mRenderSystem;
};

}

