#include "Common.hlsli"

static const int ISOMETRIC_SPRITE_BATCH_SIZE = 32;

float4x3    _IsometricSpriteWorldMatrices[ISOMETRIC_SPRITE_BATCH_SIZE];
float4      _IsometricSpriteTextureRects[ISOMETRIC_SPRITE_BATCH_SIZE];
float2      _IsometricSpriteHeight[ISOMETRIC_SPRITE_BATCH_SIZE];

#define ISOMETRIC_SPRITE_WORLD_MATRIX(i)       _IsometricSpriteWorldMatrices[i]
#define ISOMETRIC_SPRITE_TEXCOORD(i, texcoord) _IsometricSpriteTextureRects[i].xy + (texcoord) * _IsometricSpriteTextureRects[i].zw
#define ISOMETRIC_SPRITE_HEIGHT(i)             _IsometricSpriteHeight[i]
#define ISOMETRIC_SPRITE_BASE_ALTITUDE(h)      (h).r
#define ISOMETRIC_SPRITE_HEIGHT_SCALE(h)       (h).g

struct IsometricSpriteAppData
{
    float4 pos : POSITION;
    float2 texcoord : TEXCOORD0;
    float index : TEXCOORD1;
};



