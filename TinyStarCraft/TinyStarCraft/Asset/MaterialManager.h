#pragma once

#include "ResourceManager.h"

namespace TinyStarCraft
{

class EffectManager;
class Material;
class RenderSystem;

class MaterialManager : public ResourceManager
{
public:
    class MaterialResourceContainer : public Resource
    {
    public:
        MaterialResourceContainer(const std::string& name, Material* material)
            : Resource(name),
              mMaterial(material)
        {}

        ~MaterialResourceContainer();

        Material* getMaterial() const
        {
            return mMaterial;
        }

    private:
        Material* mMaterial;
    };

public:
    explicit MaterialManager(RenderSystem* renderSystem);

    ~MaterialManager();

    /**
     *	Create a material from effect.
     *
     *  @param resourceName
     *  The material's name.
     *
     *  @param effectName
     *  Name of the effect resource the material uses.
     *
     *  @return
     *  Returns nullptr if error happens.
     */
    Material* createMaterial(const std::string& resourceName, const std::string& effectName);

    /**
     *	Get material by resource name.
     *  
     *  @return
     *  Returns null if the resource is not exist.
     */
    Material* getMaterial(const std::string& resourceName) const;


private:
    EffectManager* mEffectManager;
};

};

