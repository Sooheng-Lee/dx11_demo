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

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;

    uint4 boneIDs : BONEIDS;
    float4 weights : WEIGHTS;
};

float4 VS(VS_INPUT input) : SV_POSITION
{
    float totalWeight =
        input.weights.x +
        input.weights.y +
        input.weights.z +
        input.weights.w;

    float4 skinnedPosition;

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
    }
    else
    {
        skinnedPosition =
            float4(input.position, 1.0f);
    }

    float4 worldPosition =
        mul(skinnedPosition, world);

    float4 viewPosition =
        mul(worldPosition, view);

    return mul(viewPosition, projection);
}
