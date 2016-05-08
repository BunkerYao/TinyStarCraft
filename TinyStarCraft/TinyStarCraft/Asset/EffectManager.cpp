#include "Precompiled.h"
#include "EffectManager.h"
#include "Rendering/RenderSystem.h"
#include "Utilities/Logging.h"

namespace TinyStarCraft
{

EffectManager::EffectResourceContainer::~EffectResourceContainer()
{
    mRenderSystem->destroyEffect(mEffect);
}

ID3DXEffect* EffectManager::createEffectFromFile(const std::string& resourceName, const std::string& srcFilename)
{
    // Check whether the resource name is in use.
    if (getResource(resourceName) != nullptr) {
        TINYSC_LOGLINE_ERR("Effect name \"%s\" is already in use.", srcFilename.c_str());
        return NULL;
    }

    // Create the effect
    ID3DXEffect* effect = mRenderSystem->createEffectFromFile(srcFilename, 0);
    if (!effect) 
        return NULL;

    // Wrap the effect and add to the manager.
    EffectResourceContainer* container = new EffectResourceContainer(resourceName, effect, mRenderSystem);
    addResource(container);

    return effect;
}

ID3DXEffect* EffectManager::getEffect(const std::string& resourceName) const
{
    EffectResourceContainer* container = static_cast<EffectResourceContainer*>(getResource(resourceName));

    if (container) 
        return container->getEffect();
    else 
        return NULL;
}

}


