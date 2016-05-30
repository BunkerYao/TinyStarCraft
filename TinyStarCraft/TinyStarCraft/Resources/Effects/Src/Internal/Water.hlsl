#include "Common.hlsli"

static const int WATER_TILES_BATCH_SIZE = 64;
static const float WATER_TILE_TEXCOORD_SIZE = 0.2f;

float3 _InstancePositions[WATER_TILES_BATCH_SIZE];
float2 _InstanceTexcoords[WATER_TILES_BATCH_SIZE];
float4 _DeepWaterColor = { 0.12f, 0.1f, 0.11f, 1.0f };
float4 _ShallowWaterColor = { 0.3f, 0.3f, 0.2f, 1.0f };
texture _NormalMap0;
texture _NormalMap1;

sampler NormalMap0Sampler = sampler_state
{
    texture = <_NormalMap0>;
    MinFilter = Linear;
    MagFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};

sampler NormalMap1Sampler = sampler_state
{
    texture = <_NormalMap1>;
    MinFilter = Linear;
    MagFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};

sampler RefractionSampler = sampler_state
{
    texture = <_gRefraction>;
    MinFilter = Linear;
    MagFilter = Linear;
};

sampler HeightSampler = sampler_state
{
    texture = <_gHeightBuffer>;
};

struct AppData
{
    float4 pos : POSITION;
    float2 texcoord : TEXCOORD0;
    float index : TEXCOORD1;
};

struct VertOut
{
    float4 pos : POSITION;
	float3 vView : TEXCOORD0;
    float2 geometryTexcoord : TEXCOORD1;
    float2 viewportTexcoord : TEXCOORD2;
    float altitude : TEXCOORD3;
};

VertOut VSMain(AppData i)
{
    VertOut o;

    float3 vInstancePosition = _InstancePositions[i.index];
    i.pos += float4(vInstancePosition, 0.0f);

    o.pos = float4(mul(i.pos, _gViewProjMatrix), 1.0f);
	o.vView = _gViewPoint.xyz - i.pos.xyz;
    o.geometryTexcoord = _InstanceTexcoords[i.index].xy + i.texcoord * WATER_TILE_TEXCOORD_SIZE;
    o.viewportTexcoord = CLIP_TO_VIEWPORT_TEXCOORD(o.pos);
    o.altitude = vInstancePosition.y;

    o.pos = CLIP_SPACE_HALF_PIXEL_ADJUST(o.pos);

    return o;
}

struct PixelOut
{
    float4 color : COLOR;
    float height : DEPTH;
};

PixelOut PSMain(VertOut i)
{
    PixelOut o;

    i.vView = normalize(i.vView);

    float3 vNormal0 = tex2D(NormalMap0Sampler, i.geometryTexcoord + float2(_gTime * 0.05f, -_gTime * 0.1f)).xyz * 2.0f - 1.0f;
    float3 vNormal1 = tex2D(NormalMap1Sampler, i.geometryTexcoord + float2(_gTime * 0.075f, _gTime * 0.025f)).xyz * 2.0f - 1.0f;
	float3 vNormal = normalize(vNormal0 + vNormal1);

	float3 vH = normalize(i.vView - SUN_LIGHT_DIR);
	float NdH = max(dot(vNormal, vH), 0.0f);
	float4 specular = pow(NdH, 64.0f);
    
    float NdV = max(dot(vNormal, i.vView), 0.0f);
    float4 cWaterColor = lerp(_ShallowWaterColor, _DeepWaterColor, NdV);

    i.viewportTexcoord += vNormal.xz * 0.01f;
    float fHeight = tex2D(HeightSampler, i.viewportTexcoord).r;
    float fDepth = saturate((i.altitude - fHeight) / 36.0f);
    float4 cBackground = tex2D(RefractionSampler, i.viewportTexcoord);
    float4 cRefraction = lerp(cBackground, cWaterColor, fDepth);
    
    o.color = cRefraction + specular;
	o.height = i.altitude / MAX_WORLD_HEIGHT;

    return o;
}

technique Default
{
    pass p0
    {
        CullMode = CCW;
        ZFunc = GreaterEqual;
        vertexshader = compile vs_3_0 VSMain();
        pixelshader = compile ps_3_0 PSMain();
    }
};