#pragma once

#include "ResourceManager.h"
#include "Rendering/Texture.h"
#include "Utilities/Size2.h"

namespace TinyStarCraft
{

class RenderSystem;

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
        explicit TextureResourceContainer(const std::string& name, Texture* texture, RenderSystem* renderSystem)
            : Resource(name),
              mTexture(texture),
              mRenderSystem(renderSystem)
        {}

        ~TextureResourceContainer();

        Texture* getTexture() const { return mTexture; }
    private:
        Texture* mTexture;
        RenderSystem* mRenderSystem;
    };

public:
    
    explicit TextureManager(RenderSystem* renderSystem)
        : mRenderSystem(renderSystem)
    {}

    /**
     *	Create a texture from file.
     *
     *  @param srcFilename
     *  The file name of the image file to load.
     *
     *  @param size
     *  Prefered texture size.
     *
     *  @param mipLevels
     *  The number of mipmap levels.
     *
     *  @param format
     *  Format of the texture.
     *
     *  @param isRenderTarget
     *  Whether the texture could be used as a render target or not.
     */
    Texture* createTextureFromFile(const std::string& resourceName, const std::string& fileName, 
        const Size2<UINT>& size = Size2<UINT>::ZERO(), UINT mipLevels = D3DX_DEFAULT, D3DFORMAT format = D3DFMT_UNKNOWN,
        bool isRenderTarget = false);

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

