#pragma once

#include "ResourceManager.h"

namespace TinyStarCraft
{

class EffectManager;
class Material;

class MaterialManager : private ResourceManager
{
public:
    /** Constructor */
    explicit MaterialManager(EffectManager* effectManager);

    /**
     	Create a material from effect.
    @param resourceName
        A unique material resource name.
    @param effectName
        Resource name of the effect the material uses.
    @return
        Returns nullptr if effect is not exist or resource name is in use.
     */
    Material* createMaterial(const std::string& resourceName, const std::string& effectName);

    /**
      	Retrieve a material by its resource name.
    @return
        Returns nullptr if the material with given resource name is not exist.
     */
    Material* getMaterial(const std::string& resourceName) const;

    /** 
        Restore each material's texture.
     */
    void restoreMaterialTextures();

private:
    EffectManager* mEffectManager;
};

};

