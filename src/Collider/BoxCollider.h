#pragma once
#include "Collider.h"

class BoxCollider : public Collider
{
public:
	BoxCollider();
	explicit BoxCollider(const DirectX::XMFLOAT3& size);
	~BoxCollider() override = default;

	void SetSize(const DirectX::XMFLOAT3& size) { _size = size; }
	const DirectX::XMFLOAT3& GetSize() const { return _size; }

protected:
	void CreateLineMesh(MeshData<VertexColorData>& meshData) const override;

private:
	DirectX::XMFLOAT3 _size = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
};
