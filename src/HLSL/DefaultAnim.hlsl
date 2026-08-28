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

Texture2D diffuseTexture : register(t0);
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

    return color;
}