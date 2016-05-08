#pragma once

#include "ResourceManager.h"

namespace TinyStarCraft 
{

class RenderSystem;

class EffectManager : public ResourceManager
{
public:
    class EffectResourceContainer : public Resource
    {
    public:
        EffectResourceContainer(const std::string& name, ID3DXEffect* effect, RenderSystem* renderSystem)
            : Resource(name), mEffect(effect), mRenderSystem(renderSystem)
        {}

        ~EffectResourceContainer();

        ID3DXEffect* getEffect() const
        {
            return mEffect;
        }

    private:
        ID3DXEffect* mEffect;
        RenderSystem* mRenderSystem;
    };

public:
    explicit EffectManager(RenderSystem* renderSystem)
        : mRenderSystem(renderSystem)
    {}

    /**
     *	Create an effect from file.
     *
     *  @return
     *  Returns NULL if failed.
     */
    ID3DXEffect* createEffectFromFile(const std::string& resourceName, const std::string& srcFilename);

    /**
     *	Get the effect by resource name.
     *
     *  @return
     *  Returns null if the resource is not exist.
     */
    ID3DXEffect* getEffect(const std::string& resourceName) const;

private:
    RenderSystem* mRenderSystem;
};

}

