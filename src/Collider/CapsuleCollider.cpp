#include "..\pch.h"

CapsuleCollider::CapsuleCollider()
	: Collider(eColliderType::Capsule)
{
}

CapsuleCollider::CapsuleCollider(float radius, float height)
	: Collider(eColliderType::Capsule),
	_radius(radius),
	_height(height)
{
}

void CapsuleCollider::CreateLineMesh(MeshData<VertexColorData>& meshData) const
{
	const float cylinderHeight = (std::max)(0.0f, _height - (_radius * 2.0f));
	const float halfCylinderHeight = cylinderHeight * 0.5f;

	AddRing(meshData, halfCylinderHeight);
	AddRing(meshData, -halfCylinderHeight);

	meshData.indices.push_back(0);
	meshData.indices.push_back(_segments);
	meshData.indices.push_back(_segments / 4);
	meshData.indices.push_back(_segments + (_segments / 4));
	meshData.indices.push_back(_segments / 2);
	meshData.indices.push_back(_segments + (_segments / 2));
	meshData.indices.push_back((_segments * 3) / 4);
	meshData.indices.push_back(_segments + ((_segments * 3) / 4));

	AddHemisphereArc(meshData, true, false);
	AddHemisphereArc(meshData, true, true);
	AddHemisphereArc(meshData, false, false);
	AddHemisphereArc(meshData, false, true);
}

void CapsuleCollider::AddRing(MeshData<VertexColorData>& meshData, float y) const
{
	const UINT baseIndex = static_cast<UINT>(meshData.vertices.size());
	for (UINT idx = 0; idx < _segments; ++idx)
	{
		const float angle = DirectX::XM_2PI * static_cast<float>(idx) / static_cast<float>(_segments);
		meshData.vertices.push_back({
			DirectX::XMFLOAT3(std::cos(angle) * _radius, y, std::sin(angle) * _radius),
			_color
		});
		meshData.indices.push_back(baseIndex + idx);
		meshData.indices.push_back(baseIndex + ((idx + 1) % _segments));
	}
}

void CapsuleCollider::AddHemisphereArc(MeshData<VertexColorData>& meshData, bool top, bool alongZ) const
{
	const float cylinderHeight = (std::max)(0.0f, _height - (_radius * 2.0f));
	const float centerY = (top ? 1.0f : -1.0f) * cylinderHeight * 0.5f;
	const UINT arcSegments = _segments / 2;
	const UINT baseIndex = static_cast<UINT>(meshData.vertices.size());

	for (UINT idx = 0; idx <= arcSegments; ++idx)
	{
		const float t = static_cast<float>(idx) / static_cast<float>(arcSegments);
		const float angle = t * DirectX::XM_PI;
		const float horizontal = std::cos(angle) * _radius;
		const float vertical = std::sin(angle) * _radius * (top ? 1.0f : -1.0f);

		const DirectX::XMFLOAT3 position = alongZ
			? DirectX::XMFLOAT3(0.0f, centerY + vertical, horizontal)
			: DirectX::XMFLOAT3(horizontal, centerY + vertical, 0.0f);
		meshData.vertices.push_back({ position, _color });

		if (idx < arcSegments)
		{
			meshData.indices.push_back(baseIndex + idx);
			meshData.indices.push_back(baseIndex + idx + 1);
		}
	}
}

ColliderAABB CapsuleCollider::GetLocalAABB() const
{
	return ColliderAABB{
		DirectX::XMFLOAT3(-_radius, -_height * 0.5f, -_radius),
		DirectX::XMFLOAT3(_radius, _height * 0.5f, _radius)
	};
}
