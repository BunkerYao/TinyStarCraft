
float4x3 _ViewProjMatrix;
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
    float4 weight = tex2D(ControlTextureSamp, i.controlTexcoord);
    weight /= weight.r + weight.g + weight.b;
    float4 color0 = tex2D(BlendTexture0Samp, i.blendTexcoord) * weight.r;
    float4 color1 = tex2D(BlendTexture1Samp, i.blendTexcoord) * weight.g;
    float4 color2 = tex2D(BlendTexture2Samp, i.blendTexcoord) * weight.b;
    return color0 + color1 + color2;
}

technique Default
{
    pass p0
    {
        CullMode = None;
        //FillMode = Wireframe;
        vertexshader = compile vs_2_0 VSMain();
        pixelshader = compile ps_2_0 PSMain();
    }
};