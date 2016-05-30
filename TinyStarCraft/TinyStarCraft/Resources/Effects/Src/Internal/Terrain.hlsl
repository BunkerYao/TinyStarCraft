#include "Common.hlsli"

texture _BlendTexture0;
texture _BlendTexture1;
texture _BlendTexture2;
texture _BlendTexture3;
texture _ControlTexture;

sampler BlendTexture0Samp = sampler_state
{
    texture = <_BlendTexture0>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler BlendTexture1Samp = sampler_state
{
    texture = <_BlendTexture1>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler BlendTexture2Samp = sampler_state
{
    texture = <_BlendTexture2>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler BlendTexture3Samp = sampler_state
{
    texture = <_BlendTexture3>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler ControlTextureSamp = sampler_state
{
    texture = <_ControlTexture>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
};

struct AppData
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
    float2 blendTexcoord : TEXCOORD0;
    float2 controlTexcoord : TEXCOORD1;
};

struct VertOut
{
    float4 pos : POSITION;
    float3 normal : TEXCOORD0;
    float2 blendTexcoord : TEXCOORD1;
    float2 controlTexcoord : TEXCOORD2;
    float fWorldHeight : TEXCOORD3;
};

VertOut VSMain(AppData i)
{
    VertOut o;
    o.pos = WORLD_TO_CLIP(i.pos);
    o.normal = i.normal;
    o.blendTexcoord = i.blendTexcoord;
    o.controlTexcoord = i.controlTexcoord;
	o.fWorldHeight = i.pos.y;
    return o;
}

GbufferOutput PSMain(VertOut i)
{
	GbufferOutput o;

    float4 vWeight = tex2D(ControlTextureSamp, i.controlTexcoord);
	vWeight /= vWeight.r + vWeight.g + vWeight.b;

	float4 color0 = tex2D(BlendTexture0Samp, i.blendTexcoord) * vWeight.r;
	float4 color1 = tex2D(BlendTexture1Samp, i.blendTexcoord) * vWeight.g;
	float4 color2 = tex2D(BlendTexture2Samp, i.blendTexcoord) * vWeight.b;

    o.c0 = color0 + color1 + color2;
    o.c1 = float4(i.normal * 0.5f + 0.5f, 1.0f);
	o.c2 = i.fWorldHeight;
	o.h = i.fWorldHeight / MAX_WORLD_HEIGHT;

    return o;
}

technique Default
{
    pass p0
    {
        ZFunc = GreaterEqual;
        vertexshader = compile vs_2_0 VSMain();
        pixelshader = compile ps_2_0 PSMain();
    }
};