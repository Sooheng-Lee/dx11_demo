#pragma once

enum class eColliderType
{
	Box,
	Sphere,
	Capsule
};

class Collider
{
public:
	Collider();
	virtual ~Collider() = default;

	bool Init();
	void Render(const DirectX::XMMATRIX& ownerWorldMat, const DirectX::XMMATRIX& viewMat, const DirectX::XMMATRIX& projMat);

	void SetCenter(const DirectX::XMFLOAT3& center) { _center = center; }
	const DirectX::XMFLOAT3& GetCenter() const { return _center; }
	eColliderType GetType() const { return _type; }

protected:
	explicit Collider(eColliderType type);

	virtual void CreateLineMesh(MeshData<VertexColorData>& meshData) const = 0;

private:
	void CreateVertexShader();
	void CreateInputLayout();
	void CreateVertexBuffer(const MeshData<VertexColorData>& meshData);
	void CreateIndexBuffer(const MeshData<VertexColorData>& meshData);
	void CreatePixelShader();
	void CreateConstantBuffer();

protected:
	eColliderType _type = eColliderType::Box;
	DirectX::XMFLOAT3 _center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 _color = DirectX::XMFLOAT4(0.1f, 0.85f, 1.0f, 1.0f);

private:
	std::shared_ptr<Geometry> _geometry;
	std::shared_ptr<VertexBuffer<VertexColorData>> _vertexBuffer;
	std::shared_ptr<IndexBuffer> _indexBuffer;
	std::shared_ptr<VertexShader> _vertexShader;
	std::shared_ptr<PixelShader> _pixelShader;
	std::shared_ptr<ConstantBuffer<TransformData>> _constantBuffer;
};
