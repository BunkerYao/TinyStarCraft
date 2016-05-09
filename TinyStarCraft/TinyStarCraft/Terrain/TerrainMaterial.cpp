#include "Precompiled.h"
#include "TerrainMaterial.h"
#include "Rendering/Texture.h"

namespace TinyStarCraft
{

void TerrainMaterial::setBlendTexture(size_t index, Texture* texture)
{
    static const char blendTextures[][15] = { "_BlendTexture0", "_BlendTexture1", "_BlendTexture2", "_BlendTexture3" };

    IDirect3DTexture9* d3dTexture = NULL;
    if (texture)
        d3dTexture = texture->get();

    getEffect()->SetTexture(blendTextures[index], d3dTexture);
}

void TerrainMaterial::setControlTexture(Texture* texture)
{
    IDirect3DTexture9* d3dTexture = NULL;
    if (texture)
        d3dTexture = texture->get();

    getEffect()->SetTexture("_ControlTexture", d3dTexture);
}

}