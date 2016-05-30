#include "Precompiled.h"
#include "Material.h"
#include "Effect.h"
#include "Utilities/Assert.h"
#include "Utilities/Logging.h"

namespace TinyStarCraft
{

Material::Material(ResourceManager* owner, const std::string& name, size_t ID, Effect* effect)
    : Resource(owner, name), mID(ID), mEffect(effect), mhParameterBlock(nullptr)
{
}

void Material::apply() const
{
    mEffect->getPointer()->ApplyParameterBlock(mhParameterBlock);
}

void Material::beginParameterChange()
{
    mEffect->getPointer()->BeginParameterBlock();

    if (mhParameterBlock) 
    {
        // Apply the old parameter blocks so that previouse parameter value won't be lost.
        HRESULT hr = mEffect->getPointer()->ApplyParameterBlock(mhParameterBlock);
        // Delete the previouse parameter block to save memory.
        mEffect->getPointer()->DeleteParameterBlock(mhParameterBlock);
    }
}

void Material::endParameterChange()
{
    mhParameterBlock = mEffect->getPointer()->EndParameterBlock();
}

void Material::restoreTextures()
{
    // Update the texture pointers.
    beginParameterChange();

    for (auto& it : mTextures)
        mEffect->getPointer()->SetTexture(it.first.c_str(), it.second->getPointer());

    endParameterChange();
}

}