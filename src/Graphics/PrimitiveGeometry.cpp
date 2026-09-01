#include "..\pch.h"
#include "PrimitiveGeometry.h"

void PrimitiveGeometry::AddVertex(
	MeshData<VertexTexData>& meshData,
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& normal,
	const DirectX::XMFLOAT2& uv)
{
	meshData.vertices.push_back({ position, normal, uv });
}

MeshData<VertexTexData> PrimitiveGeometry::CreatePlane(float width, float depth)
{
	MeshData<VertexTexData> meshData;
	const float halfWidth = width * 0.5f;
	const float halfDepth = depth * 0.5f;

	meshData.vertices = {
		{{-halfWidth, 0.0f, -halfDepth}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
		{{-halfWidth, 0.0f,  halfDepth}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ halfWidth, 0.0f,  halfDepth}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{ halfWidth, 0.0f, -halfDepth}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
	};

	meshData.indices = {
		0, 1, 2,
		0, 2, 3
	};

	return meshData;
}

MeshData<VertexTexData> PrimitiveGeometry::CreateCube(float width, float height, float depth)
{
	MeshData<VertexTexData> meshData;
	const float halfWidth = width * 0.5f;
	const float halfHeight = height * 0.5f;
	const float halfDepth = depth * 0.5f;

	const DirectX::XMFLOAT3 positions[8] = {
		{-halfWidth, -halfHeight, -halfDepth},
		{-halfWidth,  halfHeight, -halfDepth},
		{ halfWidth,  halfHeight, -halfDepth},
		{ halfWidth, -halfHeight, -halfDepth},
		{-halfWidth, -halfHeight,  halfDepth},
		{-halfWidth,  halfHeight,  halfDepth},
		{ halfWidth,  halfHeight,  halfDepth},
		{ halfWidth, -halfHeight,  halfDepth},
	};

	const struct Face
	{
		UINT indices[4];
		DirectX::XMFLOAT3 normal;
	} faces[6] = {
		{{4, 5, 6, 7}, { 0.0f,  0.0f,  1.0f}},
		{{3, 2, 1, 0}, { 0.0f,  0.0f, -1.0f}},
		{{0, 1, 5, 4}, {-1.0f,  0.0f,  0.0f}},
		{{7, 6, 2, 3}, { 1.0f,  0.0f,  0.0f}},
		{{1, 2, 6, 5}, { 0.0f,  1.0f,  0.0f}},
		{{4, 7, 3, 0}, { 0.0f, -1.0f,  0.0f}},
	};

	const DirectX::XMFLOAT2 uvs[4] = {
		{0.0f, 1.0f},
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
	};

	for (const Face& face : faces)
	{
		const UINT baseIndex = static_cast<UINT>(meshData.vertices.size());
		for (UINT idx = 0; idx < 4; ++idx)
		{
			AddVertex(meshData, positions[face.indices[idx]], face.normal, uvs[idx]);
		}

		meshData.indices.push_back(baseIndex + 0);
		meshData.indices.push_back(baseIndex + 1);
		meshData.indices.push_back(baseIndex + 2);
		meshData.indices.push_back(baseIndex + 0);
		meshData.indices.push_back(baseIndex + 2);
		meshData.indices.push_back(baseIndex + 3);
	}

	return meshData;
}

MeshData<VertexTexData> PrimitiveGeometry::CreateSphere(float radius, UINT sliceCount, UINT stackCount)
{
	MeshData<VertexTexData> meshData;
	sliceCount = (std::max)(sliceCount, 3u);
	stackCount = (std::max)(stackCount, 2u);

	for (UINT stack = 0; stack <= stackCount; ++stack)
	{
		const float v = static_cast<float>(stack) / static_cast<float>(stackCount);
		const float phi = DirectX::XM_PI * v;
		const float y = std::cos(phi);
		const float ringRadius = std::sin(phi);

		for (UINT slice = 0; slice <= sliceCount; ++slice)
		{
			const float u = static_cast<float>(slice) / static_cast<float>(sliceCount);
			const float theta = DirectX::XM_2PI * u;
			const DirectX::XMFLOAT3 normal(
				ringRadius * std::cos(theta),
				y,
				ringRadius * std::sin(theta));
			const DirectX::XMFLOAT3 position(
				normal.x * radius,
				normal.y * radius,
				normal.z * radius);

			AddVertex(meshData, position, normal, DirectX::XMFLOAT2(u, v));
		}
	}

	const UINT ringVertexCount = sliceCount + 1;
	for (UINT stack = 0; stack < stackCount; ++stack)
	{
		for (UINT slice = 0; slice < sliceCount; ++slice)
		{
			const UINT baseIndex = stack * ringVertexCount + slice;
			meshData.indices.push_back(baseIndex);
			meshData.indices.push_back(baseIndex + ringVertexCount);
			meshData.indices.push_back(baseIndex + ringVertexCount + 1);

			meshData.indices.push_back(baseIndex);
			meshData.indices.push_back(baseIndex + ringVertexCount + 1);
			meshData.indices.push_back(baseIndex + 1);
		}
	}

	return meshData;
}

MeshData<VertexTexData> PrimitiveGeometry::CreateCapsule(float radius, float height, UINT sliceCount, UINT hemisphereStackCount)
{
	MeshData<VertexTexData> meshData;
	sliceCount = (std::max)(sliceCount, 3u);
	hemisphereStackCount = (std::max)(hemisphereStackCount, 2u);

	const float cylinderHeight = (std::max)(0.0f, height - radius * 2.0f);
	const float halfCylinderHeight = cylinderHeight * 0.5f;
	const UINT verticalRingCount = hemisphereStackCount * 2 + 1;

	for (UINT ring = 0; ring <= verticalRingCount; ++ring)
	{
		const float t = static_cast<float>(ring) / static_cast<float>(verticalRingCount);
		const float phi = DirectX::XM_PI * t;
		const float sphereY = std::cos(phi);
		const float ringRadius = std::sin(phi);
		const bool isTop = sphereY >= 0.0f;
		const float y = sphereY * radius + (isTop ? halfCylinderHeight : -halfCylinderHeight);

		for (UINT slice = 0; slice <= sliceCount; ++slice)
		{
			const float u = static_cast<float>(slice) / static_cast<float>(sliceCount);
			const float theta = DirectX::XM_2PI * u;
			const DirectX::XMFLOAT3 normal(
				ringRadius * std::cos(theta),
				sphereY,
				ringRadius * std::sin(theta));
			const DirectX::XMFLOAT3 position(
				normal.x * radius,
				y,
				normal.z * radius);

			AddVertex(meshData, position, normal, DirectX::XMFLOAT2(u, t));
		}
	}

	const UINT ringVertexCount = sliceCount + 1;
	for (UINT ring = 0; ring < verticalRingCount; ++ring)
	{
		for (UINT slice = 0; slice < sliceCount; ++slice)
		{
			const UINT baseIndex = ring * ringVertexCount + slice;
			meshData.indices.push_back(baseIndex);
			meshData.indices.push_back(baseIndex + ringVertexCount);
			meshData.indices.push_back(baseIndex + ringVertexCount + 1);

			meshData.indices.push_back(baseIndex);
			meshData.indices.push_back(baseIndex + ringVertexCount + 1);
			meshData.indices.push_back(baseIndex + 1);
		}
	}

	return meshData;
}
