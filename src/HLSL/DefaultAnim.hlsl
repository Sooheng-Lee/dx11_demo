#define MAX_BONES 128

cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

cbuffer BoneBuffer : register(b1)
{
    matrix bones[MAX_BONES];
};

cbuffer LightBuffer : register(b0)
{
    float4 directionalLightDirection;
    float4 directionalLightColor;
    float4 ambientColor;
    uint useLight;
    uint useNormalTexture;
    uint useRoughnessTexture;
    uint useMetallicTexture;
};

Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D roughnessTexture : register(t2);
Texture2D metallicTexture : register(t3);
SamplerState samplerState : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;

    uint4 boneIDs : BONEIDS;
    float4 weights : WEIGHTS;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};


VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    float totalWeight =
        input.weights.x +
        input.weights.y +
        input.weights.z +
        input.weights.w;

    float4 skinnedPosition;
    float3 skinnedNormal;

    if (totalWeight > 0.0001f)
    {
        matrix skinMatrix =
            bones[input.boneIDs.x] * input.weights.x
            + bones[input.boneIDs.y] * input.weights.y
            + bones[input.boneIDs.z] * input.weights.z
            + bones[input.boneIDs.w] * input.weights.w;

        skinnedPosition =
            mul(
                float4(input.position, 1.0f),
                skinMatrix
            );

        skinnedNormal =
            mul(
                float4(input.normal, 0.0f),
                skinMatrix
            ).xyz;
    }
    else
    {
        skinnedPosition =
            float4(input.position, 1.0f);

        skinnedNormal =
            input.normal;
    }

    float4 worldPosition =
        mul(skinnedPosition, world);

    float4 viewPosition =
        mul(worldPosition, view);

    output.worldPosition =
        worldPosition.xyz;

    output.position =
        mul(
            viewPosition,
            projection
        );

    output.normal =
        normalize(
            mul(
                float4(skinnedNormal, 0.0f),
                world
            ).xyz
        );

    output.uv =
        input.uv;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 color =
        diffuseTexture.Sample(
            samplerState,
            input.uv
        );

    if (useLight != 0)
    {
        float3 normal = normalize(input.normal);
        if (useNormalTexture != 0)
        {
            normal = normalize(normalTexture.Sample(samplerState, input.uv).xyz * 2.0f - 1.0f);
        }

        float3 lightDirection = normalize(-directionalLightDirection.xyz);
        float diffuseFactor = saturate(dot(normal, lightDirection) * 0.5f + 0.5f);
        float3 lightColor = ambientColor.rgb + directionalLightColor.rgb * diffuseFactor;

        float roughness = useRoughnessTexture != 0
            ? roughnessTexture.Sample(samplerState, input.uv).r
            : 1.0f;
        float metallic = useMetallicTexture != 0
            ? metallicTexture.Sample(samplerState, input.uv).r
            : 0.0f;
        float3 viewDirection = normalize(-input.worldPosition);
        float3 halfVector = normalize(lightDirection + viewDirection);
        float specularPower = lerp(128.0f, 8.0f, saturate(roughness));
        float specularFactor = pow(saturate(dot(normal, halfVector)), specularPower);
        float3 specularColor = lerp(float3(0.04f, 0.04f, 0.04f), color.rgb, saturate(metallic));

        color.rgb *= saturate(lightColor);
        color.rgb += specularColor * directionalLightColor.rgb * specularFactor * (1.0f - saturate(roughness));
    }

    return color;
}
