#pragma once

#include "Effect.h"
#include "Resource.h"
#include "Texture.h"

namespace TinyStarCraft
{

class Effect;

/**
	Material defines an "instance" of an effect.
@remarks
    Different material of one same effect has different parameter values, e.g. colors,
    textures, etc. Material class uses parameter block to capture parameter values. Once
    a material is going to be used, the material object will apply its own parameter block
    to set all its parameter values to the effect.
 */
class Material : public Resource
{
public:
    /** Constructor */
    Material(ResourceManager* owner, const std::string& name, size_t ID, Effect* effect);

    /** Check if two material are the same by comparing the IDs of them. */
    bool operator==(const Material& other) const { return mID == other.mID; }

    /**	Compares two material by comparing the IDs of them. */
    bool operator<(const Material& other) const { return mID < other.mID; }

    /**	Get the referenced effect interface. */
    Effect* getEffect() const { return mEffect; }

    /** Get the ID of this material. */
    size_t getID() const { return mID; }

    /**	Apply this material's parameter to the effect. */
    void apply() const;

    /** 
        Begin to change parameter of this material.
    @remarks
        Call Material::endParameterChange to stop parameter change and save the parameters.
        Any setXXX method should be called between Material::beginParameterChange and 
        Material::endParameterChange.
     */
    void beginParameterChange();

    /**
        Stop to change parameter of this material.
    @remarks
        Stop parameter change and save the current parameter values. When Material::apply is called, current
        parameter values will be applied to the effect.
    */
    void endParameterChange();

    /** Set a bool value. */
    void setBool(const std::string& name, BOOL value)
    {
        mEffect->getPointer()->SetBool(name.c_str(), value);
    }

    /** Set a float value. */
    void setFloat(const std::string& name, float value)
    {
        mEffect->getPointer()->SetFloat(name.c_str(), value);
    }

    /** Set a int value. */
    void setInt(const std::string& name, int value)
    {
        mEffect->getPointer()->SetInt(name.c_str(), value);
    }

    /** Set a matrix. */
    void setMatrix(const std::string& name, const D3DXMATRIX* value)
    {
        mEffect->getPointer()->SetMatrix(name.c_str(), value);
    }

    /** Set a vector. */
    void setVector(const std::string& name, const D3DXVECTOR4* value)
    {
        mEffect->getPointer()->SetVector(name.c_str(), value);
    }

    /** Set a color. */
    void setColor(const std::string& name, const D3DXCOLOR* value)
    {
        mEffect->getPointer()->SetVector(name.c_str(), (D3DXVECTOR4*)value);
    }

    /* Set a texture. */
    void setTexture(const std::string& name, const Texture* texture)
    {
        mTextures[name] = texture;

        mEffect->getPointer()->SetTexture(name.c_str(), texture->getPointer());
    }

    /**
        Restore all textures. 
    @remarks
        After device is reset, texture might be reloaded which causes the old texture pointer to be invalid.
        Call this method to update the pointers to the new loaded textures.
     */
    void restoreTextures();

private:
    Effect* mEffect;
    size_t mID;
    D3DXHANDLE mhParameterBlock;

    // Every texture being set by calling Material::setTexture are captured in this map.
    // It is used for texture restoration.
    std::unordered_map<std::string, const Texture*> mTextures;
};

}
