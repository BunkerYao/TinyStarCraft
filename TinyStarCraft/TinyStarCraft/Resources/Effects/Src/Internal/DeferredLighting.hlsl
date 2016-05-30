#include "Common.hlsli"

float4 _AmbientColor = { 0.1f, 0.1f, 0.1f, 0.1f };
float4 _LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };

sampler Gbuffer0Samp = sampler_state
{
    texture = <_gGbuffer0>;
};

sampler Gbuffer1Samp = sampler_state
{
    texture = <_gGbuffer1>;
};

sampler HeightSamp = sampler_state
{
    texture = <_gHeightBuffer>;
};

struct AppData
{
    float4 pos : POSITION;
};

struct VertOut
{
	float4 pos : POSITION;
	float2 viewportTexcoord : TEXCOORD;
};

VertOut VSMain(AppData i)
{
	VertOut o;
	o.viewportTexcoord = CLIP_TO_VIEWPORT_TEXCOORD(i.pos);
    o.pos = CLIP_SPACE_HALF_PIXEL_ADJUST(i.pos);
	return o;
}

float4 PSMain(VertOut i) : COLOR
{
	float3 cAlbedo = tex2D(Gbuffer0Samp, i.viewportTexcoord).rgb;
	float3 vNormal = tex2D(Gbuffer1Samp, i.viewportTexcoord).rgb * 2.0f - 1.0f;

	float NdL = max(dot(-SUN_LIGHT_DIR, vNormal), 0.0f);
    float4 cDiffuse = float4(cAlbedo * NdL, 1.0f);

	return cDiffuse;
}

technique Default
{
	pass p0
	{
		ZEnable = false;
		VertexShader = compile vs_2_0 VSMain();
		PixelShader = compile ps_2_0 PSMain();
	}
};
