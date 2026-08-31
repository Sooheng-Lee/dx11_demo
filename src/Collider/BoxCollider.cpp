#include "..\pch.h"

BoxCollider::BoxCollider()
	: Collider(eColliderType::Box)
{
}

BoxCollider::BoxCollider(const DirectX::XMFLOAT3& size)
	: Collider(eColliderType::Box),
	_size(size)
{
}

void BoxCollider::CreateLineMesh(MeshData<VertexColorData>& meshData) const
{
	const float halfX = _size.x * 0.5f;
	const float halfY = _size.y * 0.5f;
	const float halfZ = _size.z * 0.5f;

	meshData.vertices = {
		{{-halfX, -halfY, -halfZ}, _color},
		{{-halfX,  halfY, -halfZ}, _color},
		{{ halfX,  halfY, -halfZ}, _color},
		{{ halfX, -halfY, -halfZ}, _color},
		{{-halfX, -halfY,  halfZ}, _color},
		{{-halfX,  halfY,  halfZ}, _color},
		{{ halfX,  halfY,  halfZ}, _color},
		{{ halfX, -halfY,  halfZ}, _color},
	};

	meshData.indices = {
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};
}
