#include "Precompiled.h"
#include "MaterialManager.h"
#include "EffectManager.h"
#include "Material.h"
#include "Utilities/Logging.h"

namespace TinyStarCraft
{

MaterialManager::MaterialManager(EffectManager* effectManager)
    : mEffectManager(effectManager)
{
}

Material* MaterialManager::createMaterial(const std::string& resourceName, const std::string& effectName)
{
    // Check whether the resource name is in use.
    if (getResource(resourceName) != nullptr) 
    {
        TINYSC_LOGLINE_ERR("Resource name \"%s\" is already in use.", resourceName.c_str());
        return nullptr;
    }

    // Retrieve the effect.
    Effect* effect = mEffectManager->getEffect(effectName);
    if (effect == nullptr) 
    {
        TINYSC_LOGLINE_ERR("Effect \"%s\" is not exist.", effectName.c_str());
        return nullptr;
    }

    // Create the material.
    // Use the material name's hash number as the material ID.
    std::hash<std::string> hashFunc;
    Material* material = new Material(this, resourceName, hashFunc(resourceName), effect);
    addResource(material);

    return material;
}

Material* MaterialManager::getMaterial(const std::string& resourceName) const
{
    return static_cast<Material*>(getResource(resourceName));
}

void MaterialManager::restoreMaterialTextures()
{
    // Restore all material's textures.
    std::vector<std::string> resourceNames = getResourceNames();
    for (auto& it : resourceNames)
    {
        Material* material = getMaterial(it);
        material->restoreTextures();
    }
}

}

