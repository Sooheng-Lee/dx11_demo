#pragma once
#include <DirectXMath.h>

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