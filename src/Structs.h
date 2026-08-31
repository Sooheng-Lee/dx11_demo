#pragma once
#include <DirectXMath.h>

constexpr UINT MAX_BONE_INFLUENCE = 4;

struct VertexColorData
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};

struct VertexTexData
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

struct VertexAnimData
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;

	UINT boneIDs[MAX_BONE_INFLUENCE] =
	{
		0, 0, 0, 0
	};

	float boneWeights[MAX_BONE_INFLUENCE] =
	{
		0.0f, 0.0f, 0.0f, 0.0f
	};

	void AddBoneData(UINT boneID, float weight)
	{
		for (UINT idx = 0; idx < MAX_BONE_INFLUENCE; ++idx)
		{
			if (boneWeights[idx] == 0.0f)
			{
				boneIDs[idx] = boneID;
				boneWeights[idx] = weight;

				return;
			}
		}
	}
};

template <typename T>
struct MeshData
{
public:
	std::vector<T> vertices;
	std::vector<UINT> indices;
};

struct TransformData
{
	DirectX::XMMATRIX worldMat;
	DirectX::XMMATRIX viewMat;
	DirectX::XMMATRIX projMat;
};

struct BoneTransformData
{
	DirectX::XMMATRIX boneMats[128];
};

struct LightData
{
	DirectX::XMFLOAT4 directionalLightDirection = DirectX::XMFLOAT4(0.3f, -1.0f, 0.3f, 0.0f);
	DirectX::XMFLOAT4 directionalLightColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 ambientColor = DirectX::XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	UINT useLight = 1;
	UINT useNormalTexture = 0;
	UINT useRoughnessTexture = 0;
	UINT useMetallicTexture = 0;
};

struct BoneInfo
{
	UINT id = -1;
	DirectX::XMFLOAT4X4 offsetMatrix;
};
