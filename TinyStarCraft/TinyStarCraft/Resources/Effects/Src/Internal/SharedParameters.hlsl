#include "Common.hlsli"

float4 VSMain(float4 pos : POSITION) : POSITION
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

float4 PSMain() : COLOR
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

technique Default
{
    pass p0
    {
        VertexShader = compile vs_2_0 VSMain();
        PixelShader = compile ps_2_0 PSMain();
    }
};