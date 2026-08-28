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
	DirectX::XMFLOAT2 uv;
};

struct TransformData
{
	DirectX::XMMATRIX worldMat;
	DirectX::XMMATRIX viewMat;
	DirectX::XMMATRIX projMat;
};