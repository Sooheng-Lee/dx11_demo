cbuffer TransformData : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

Texture2D _texture : register(t0);
sampler _sampler : register(s0);

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    float4x4 vp = mul(view, proj);
    output.position = mul(mul(float4(input.position, 1.0f), world), vp);
    output.normal = normalize(output.position.xyz);
    output.uv = input.uv;
    return output;
}

float4 PS(PS_INPUT input) : SV_TARGET
{
    return _texture.Sample(_sampler, input.uv);
}