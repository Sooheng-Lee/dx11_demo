#pragma once
#include "Collider.h"

class SphereCollider : public Collider
{
public:
	SphereCollider();
	explicit SphereCollider(float radius);
	~SphereCollider() override = default;

	void SetRadius(float radius) { _radius = radius; }
	float GetRadius() const { return _radius; }

protected:
	void CreateLineMesh(MeshData<VertexColorData>& meshData) const override;

private:
	float _radius = 0.5f;
	UINT _segments = 32;
};
