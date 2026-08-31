#pragma once
#include "Collider.h"

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider();
	CapsuleCollider(float radius, float height);
	~CapsuleCollider() override = default;

	void SetRadius(float radius) { _radius = radius; }
	float GetRadius() const { return _radius; }
	void SetHeight(float height) { _height = height; }
	float GetHeight() const { return _height; }

protected:
	void CreateLineMesh(MeshData<VertexColorData>& meshData) const override;

private:
	void AddRing(MeshData<VertexColorData>& meshData, float y) const;
	void AddHemisphereArc(MeshData<VertexColorData>& meshData, bool top, bool alongZ) const;

private:
	float _radius = 0.35f;
	float _height = 1.8f;
	UINT _segments = 32;
};
