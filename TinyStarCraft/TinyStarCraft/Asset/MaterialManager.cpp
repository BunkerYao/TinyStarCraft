#include "Precompiled.h"
#include "MaterialManager.h"
#include "EffectManager.h"
#include "Rendering/Material.h"
#include "Utilities/Logging.h"

namespace TinyStarCraft
{

MaterialManager::MaterialManager(RenderSystem* renderSystem)
    : mEffectManager(nullptr)
{
    mEffectManager = new EffectManager(renderSystem);

    // Create the terrain effect.
    mEffectManager->createEffectFromFile("terrain", "./Resources/Effects/Terrain.hlsl");
}

MaterialManager::~MaterialManager()
{
    // Must destroy all materials before destroy effects.
    destroyAllResources();

    delete mEffectManager;
}

Material* MaterialManager::createMaterial(const std::string& resourceName, const std::string& effectName)
{
    // Check whether the resource name is in use.
    if (getResource(resourceName) != nullptr) {
        TINYSC_LOGLINE_ERR("Material name \"%s\" is already in use.", resourceName.c_str());
        return nullptr;
    }

    // Get the effect.
    ID3DXEffect* effect = mEffectManager->getEffect(effectName);
    if (effect == nullptr) {
        TINYSC_LOGLINE_ERR("Effect \"%s\" is not exist.", effectName.c_str());
        return nullptr;
    }

    // Create the material.
    // Use the material name's hash number as the material id.
    std::hash<std::string> hashFunc;
    Material* material = new Material(effect, (int)hashFunc(resourceName));
    if (material == nullptr) {
        return nullptr;
    }

    // Wrap the material and add to the resource manager.
    MaterialResourceContainer* container = new MaterialResourceContainer(resourceName, material);
    addResource(container);

    return material;
}

Material* MaterialManager::getMaterial(const std::string& resourceName) const
{
    MaterialResourceContainer* container = static_cast<MaterialResourceContainer*>(getResource(resourceName));

    if (container != nullptr) 
        return container->getMaterial();
    else 
        return nullptr;
}

MaterialManager::MaterialResourceContainer::~MaterialResourceContainer()
{
    delete mMaterial;
}

}

