#include "Precompiled.h"
#include "EffectManager.h"
#include "Effect.h"
#include "Rendering/RenderSystem.h"
#include "Utilities/Assert.h"
#include "Utilities/DebugOutput.h"
#include "Utilities/Logging.h"

namespace TinyStarCraft
{

EffectManager::EffectManager(IDirect3DDevice9* D3DDevice) 
    : mD3DDevice(D3DDevice), mEffectPool(nullptr)
{
}

EffectManager::~EffectManager()
{
    destroyAllResources();

    ULONG number = mEffectPool->Release();
    TINYSC_ASSERT(number == 0, "Effect pool is not released successfully.");
}

bool EffectManager::initialize()
{
    HRESULT hr = ::D3DXCreateEffectPool(&mEffectPool);
    if (FAILED(hr)) 
    {
        TINYSC_LOGLINE_ERR("D3DXCreateEffectPool failed 0x%08x %s.", hr, ::DXGetErrorString(hr));
        return false;
    }
    return true;
}

Effect* EffectManager::createEffectFromFile(const std::string& resourceName, const std::string& srcFilename)
{
    // Check if the resource name is in use.
    if (getResource(resourceName) != nullptr) 
    {
        TINYSC_LOGLINE_ERR("Resource name \"%s\" is already in use.", resourceName.c_str());
        return nullptr;
    }

    ID3DXBuffer* compilationError = nullptr;
    Effect* effect = new Effect(this, resourceName);
    bool result = effect->createFromFile(mD3DDevice, srcFilename, 0, mEffectPool, &compilationError);

    if (compilationError) 
    {
        // Output compilation messages to the output window.
        OutputDebugStringF((char*)compilationError->GetBufferPointer());
        compilationError->Release();
    }

    if (!result) 
    {
        TINYSC_LOGLINE_ERR("Failed to create effect from file.");
        delete effect;
        return nullptr;
    }

    addResource(effect);

    return effect;
}

Effect* EffectManager::getEffect(const std::string& resourceName) const
{
    return static_cast<Effect*>(getResource(resourceName));
}

void EffectManager::onDeviceLost()
{
    std::vector<std::string> resourceNames = getResourceNames();
    for (auto& it : resourceNames) 
    {
        Effect* effect = getEffect(it);
        effect->getPointer()->OnLostDevice();
    }
}

void EffectManager::onDeviceReset()
{
    std::vector<std::string> resourceNames = getResourceNames();
    for (auto& it : resourceNames) 
    {
        Effect* effect = getEffect(it);
        effect->getPointer()->OnResetDevice();
    }
}

}


