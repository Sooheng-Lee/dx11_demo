#pragma once

class PrimitiveGeometry
{
public:
	static MeshData<VertexTexData> CreatePlane(float width = 1.0f, float depth = 1.0f);
	static MeshData<VertexTexData> CreateCube(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
	static MeshData<VertexTexData> CreateSphere(float radius = 0.5f, UINT sliceCount = 32, UINT stackCount = 16);
	static MeshData<VertexTexData> CreateCapsule(float radius = 0.35f, float height = 1.8f, UINT sliceCount = 32, UINT hemisphereStackCount = 8);

private:
	static void AddVertex(
		MeshData<VertexTexData>& meshData,
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT3& normal,
		const DirectX::XMFLOAT2& uv);
};
