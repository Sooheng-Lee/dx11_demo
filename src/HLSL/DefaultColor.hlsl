cbuffer TransformData : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    float4 worldPosition = mul(float4(input.position, 1.0f), world);
    float4 viewPosition = mul(worldPosition, view);
    output.position = mul(viewPosition, proj);
    output.color = input.color;
    return output;
}

float4 PS(PS_INPUT input) : SV_TARGET
{
    return input.color;
}
