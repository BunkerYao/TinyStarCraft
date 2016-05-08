
float4x3 _ViewProjMatrix;

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
};

VertOut VSMain(AppData i)
{
    VertOut o;
    o.pos = float4(mul(i.pos, _ViewProjMatrix), 1.0f);
    o.normal = i.normal;
    o.blendTexcoord = i.blendTexcoord;
    o.controlTexcoord = i.controlTexcoord;
    return o;
}

float4 PSMain(VertOut i) : COLOR
{
    return float4(i.controlTexcoord.xy, 0.0f, 1.0f);
}

technique Default
{
    pass p0
    {
        CullMode = None;
        ZEnable = False;
        FillMode = WireFrame;
        vertexshader = compile vs_2_0 VSMain();
        pixelshader = compile ps_2_0 PSMain();
    }
};