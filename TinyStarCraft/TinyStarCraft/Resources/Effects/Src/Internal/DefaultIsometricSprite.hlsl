#include "IsometricSprite.hlsli"

texture _Albedo;
texture _Normal;
texture _Height;

sampler AlbedoSampler = sampler_state
{
    texture = <_Albedo>;
};

sampler NormalSampler = sampler_state
{
    texture = <_Normal>;
};

sampler HeightSampler = sampler_state
{
    texture = <_Height>;
};

struct VertOut
{
    float4 pos : POSITION;
    float2 texcoord : TEXCOORD;
    float2 height : TEXCOORD1;
};

VertOut VSIsometricSpriteDefault(IsometricSpriteAppData i)
{
    VertOut o;

    float4 vWorldPos = float4(mul(i.pos, ISOMETRIC_SPRITE_WORLD_MATRIX(i.index)), 1.0f);
    o.pos = WORLD_TO_CLIP(vWorldPos);
    o.texcoord = ISOMETRIC_SPRITE_TEXCOORD(i.index, i.texcoord);
    //o.height = ISOMETRIC_SPRITE_HEIGHT(i.index);
    o.height = vWorldPos.y;

    return o;
}

GbufferOutput PSIsometricSpriteDefault(VertOut i)
{
    GbufferOutput o;

    o.c0 = tex2D(AlbedoSampler, i.texcoord);
    o.c1 = tex2D(NormalSampler, i.texcoord);
    o.c2 = tex2D(HeightSampler, i.texcoord).r * ISOMETRIC_SPRITE_HEIGHT_SCALE(i.height) + ISOMETRIC_SPRITE_BASE_ALTITUDE(i.height);
    o.h = o.c2.r / MAX_WORLD_HEIGHT;

    return o;
}

technique Default
{
    pass p0
    {
        ZFunc = Greater;
        VertexShader = compile vs_2_0 VSIsometricSpriteDefault();
        PixelShader = compile ps_2_0 PSIsometricSpriteDefault();
    }
};
