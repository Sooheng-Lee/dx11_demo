cbuffer TransformData : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};

cbuffer TextureMaterialData : register(b0)
{
    float4 materialColor;
    uint useTexture;
    float3 materialPadding;
};

cbuffer LightData : register(b1)
{
    float4 directionalLightDirection;
    float4 directionalLightColor;
    float4 ambientColor;
    uint useLight;
    uint useNormalTexture;
    uint useRoughnessTexture;
    uint useMetallicTexture;
};

cbuffer ShadowData : register(b2)
{
    float4x4 lightViewProj;
    uint useShadow;
    float3 shadowPadding;
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
    float3 worldPosition : POSITION;
    float4 lightPosition : TEXCOORD1;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

Texture2D _texture : register(t0);
Texture2D shadowTexture : register(t1);
sampler _sampler : register(s0);
SamplerState shadowSampler : register(s1);

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    float4x4 vp = mul(view, proj);
    float4 worldPosition = mul(float4(input.position, 1.0f), world);
    output.position = mul(worldPosition, vp);
    output.worldPosition = worldPosition.xyz;
    output.lightPosition = mul(worldPosition, lightViewProj);
    output.normal = normalize(mul(float4(input.normal, 0.0f), world).xyz);
    output.uv = input.uv;
    return output;
}

float4 PS(PS_INPUT input) : SV_TARGET
{
    float4 color = materialColor;
    if (useTexture != 0)
    {
        color = _texture.Sample(_sampler, input.uv);
    }

    if (useLight != 0)
    {
        float3 normal = normalize(input.normal);
        float3 lightDirection = normalize(-directionalLightDirection.xyz);
        float diffuseFactor = saturate(dot(normal, lightDirection) * 0.5f + 0.5f);
        float3 lightColor = ambientColor.rgb + directionalLightColor.rgb * diffuseFactor;
        if (useShadow != 0)
        {
            float3 shadowCoord = input.lightPosition.xyz / input.lightPosition.w;
            shadowCoord.x = shadowCoord.x * 0.5f + 0.5f;
            shadowCoord.y = shadowCoord.y * -0.5f + 0.5f;

            if (shadowCoord.x >= 0.0f && shadowCoord.x <= 1.0f &&
                shadowCoord.y >= 0.0f && shadowCoord.y <= 1.0f &&
                shadowCoord.z >= 0.0f && shadowCoord.z <= 1.0f)
            {
                float shadowDepth = shadowTexture.Sample(shadowSampler, shadowCoord.xy).r;
                float currentDepth = shadowCoord.z - 0.0025f;
                if (currentDepth > shadowDepth)
                {
                    lightColor *= 0.45f;
                }
            }
        }
        color.rgb *= saturate(lightColor);
    }

    return color;
}
