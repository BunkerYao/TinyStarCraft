#include "Precompiled.h"
#include "Effect.h"
#include "Utilities/Assert.h"
#include "Utilities/Logging.h"

namespace TinyStarCraft
{

Effect::Effect(ResourceManager* owner, const std::string& name)
    : Resource(owner, name), mEffect(nullptr)
{
}

Effect::~Effect()
{
    if (mEffect) 
    {
        ULONG number = mEffect->Release();
        TINYSC_ASSERT(number == 0, "Effect released unsuccessfully.");
    }
}

bool Effect::createFromFile(IDirect3DDevice9* D3DDevice, const std::string& srcFilename, DWORD flags, ID3DXEffectPool* pool,
    ID3DXBuffer** compilationErrors)
{
    TINYSC_ASSERT(mEffect == nullptr, "Effect has been created before.");

    HRESULT hr = ::D3DXCreateEffectFromFile(
        D3DDevice,
        srcFilename.c_str(),
        nullptr,
        nullptr,
        flags,
        pool,
        &mEffect,
        compilationErrors
        );

    if (FAILED(hr)) 
    {
        TINYSC_LOGLINE_ERR("::D3DXCreateEffectFromFile failed 0x%08x %s.", hr, ::DXGetErrorString(hr));
        return false;
    }

    return true;
}

}