#pragma once

#include "Rendering/Material.h"

namespace TinyStarCraft
{

class Texture;

class TerrainMaterial : public Material
{
public:
    TerrainMaterial(ID3DXEffect* effect, int Id)
        : Material(effect, Id)
    {}

    /**
     *	Set a blend texture.
     *
     *  @param index
     *  Could be integer between 0 to 3.
     *
     *  @param texture
     *  The blend texture to use.
     *
     *  @remarks
     *  There are 4 blend textures. 
     */
    void setBlendTexture(size_t index, Texture* texture);

    /**
     *	Set the control texture.
     */
    void setControlTexture(Texture* texture);
};

}

