#include "..\pch.h"

SphereCollider::SphereCollider()
	: Collider(eColliderType::Sphere)
{
}

SphereCollider::SphereCollider(float radius)
	: Collider(eColliderType::Sphere),
	_radius(radius)
{
}

void SphereCollider::CreateLineMesh(MeshData<VertexColorData>& meshData) const
{
	const UINT ringCount = 3;
	meshData.vertices.reserve(_segments * ringCount);
	meshData.indices.reserve(_segments * ringCount * 2);

	for (UINT ring = 0; ring < ringCount; ++ring)
	{
		const UINT baseIndex = static_cast<UINT>(meshData.vertices.size());
		for (UINT idx = 0; idx < _segments; ++idx)
		{
			const float angle = DirectX::XM_2PI * static_cast<float>(idx) / static_cast<float>(_segments);
			const float sinValue = std::sin(angle) * _radius;
			const float cosValue = std::cos(angle) * _radius;

			DirectX::XMFLOAT3 position;
			if (ring == 0)
			{
				position = DirectX::XMFLOAT3(cosValue, 0.0f, sinValue);
			}
			else if (ring == 1)
			{
				position = DirectX::XMFLOAT3(cosValue, sinValue, 0.0f);
			}
			else
			{
				position = DirectX::XMFLOAT3(0.0f, cosValue, sinValue);
			}

			meshData.vertices.push_back({ position, _color });
			meshData.indices.push_back(baseIndex + idx);
			meshData.indices.push_back(baseIndex + ((idx + 1) % _segments));
		}
	}
}

ColliderAABB SphereCollider::GetLocalAABB() const
{
	return ColliderAABB{
		DirectX::XMFLOAT3(-_radius, -_radius, -_radius),
		DirectX::XMFLOAT3(_radius, _radius, _radius)
	};
}
