#include "..\pch.h"

Collider::Collider()
{
}

Collider::Collider(eColliderType type)
	: _type(type)
{
}

bool Collider::Init()
{
	MeshData<VertexColorData> meshData;
	CreateLineMesh(meshData);

	if (meshData.vertices.empty() || meshData.indices.empty())
	{
		return false;
	}

	CreateVertexShader();
	CreateInputLayout();
	CreateVertexBuffer(meshData);
	CreateIndexBuffer(meshData);
	CreatePixelShader();
	CreateConstantBuffer();

	return true;
}

void Collider::Render(const DirectX::XMMATRIX& ownerWorldMat, const DirectX::XMMATRIX& viewMat, const DirectX::XMMATRIX& projMat)
{
	if (_vertexBuffer == nullptr || _indexBuffer == nullptr)
	{
		return;
	}

	const DirectX::XMMATRIX localMat =
		DirectX::XMMatrixTranslation(_center.x, _center.y, _center.z);
	const DirectX::XMMATRIX worldMat = localMat * ownerWorldMat;

	TransformData transformData;
	transformData.worldMat = DirectX::XMMatrixTranspose(worldMat);
	transformData.viewMat = DirectX::XMMatrixTranspose(viewMat);
	transformData.projMat = DirectX::XMMatrixTranspose(projMat);
	_constantBuffer->Update(&transformData);

	UINT offset = 0;
	Graphic::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Graphic::GetInstance()->GetDeviceContext()->IASetInputLayout(_geometry->GetComPtr().Get());
	Graphic::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), _vertexBuffer->GetStridePtr(), &offset);
	Graphic::GetInstance()->GetDeviceContext()->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, offset);
	Graphic::GetInstance()->GetDeviceContext()->VSSetShader(_vertexShader->GetComPtr().Get(), nullptr, 0);
	Graphic::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(0, 1, _constantBuffer->GetComPtr().GetAddressOf());
	Graphic::GetInstance()->GetDeviceContext()->PSSetShader(_pixelShader->GetComPtr().Get(), nullptr, 0);
	Graphic::GetInstance()->GetDeviceContext()->DrawIndexed(_indexBuffer->GetCount(), 0, 0);
}

ColliderAABB Collider::GetWorldAABB(const DirectX::XMMATRIX& ownerWorldMat) const
{
	const ColliderAABB localAABB = GetLocalAABB();
	const DirectX::XMMATRIX localMat =
		DirectX::XMMatrixTranslation(_center.x, _center.y, _center.z);
	const DirectX::XMMATRIX worldMat = localMat * ownerWorldMat;

	const DirectX::XMFLOAT3 corners[8] = {
		{localAABB.min.x, localAABB.min.y, localAABB.min.z},
		{localAABB.min.x, localAABB.min.y, localAABB.max.z},
		{localAABB.min.x, localAABB.max.y, localAABB.min.z},
		{localAABB.min.x, localAABB.max.y, localAABB.max.z},
		{localAABB.max.x, localAABB.min.y, localAABB.min.z},
		{localAABB.max.x, localAABB.min.y, localAABB.max.z},
		{localAABB.max.x, localAABB.max.y, localAABB.min.z},
		{localAABB.max.x, localAABB.max.y, localAABB.max.z},
	};

	ColliderAABB worldAABB;
	worldAABB.min = DirectX::XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	worldAABB.max = DirectX::XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (const DirectX::XMFLOAT3& corner : corners)
	{
		DirectX::XMVECTOR worldCorner = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&corner), worldMat);
		DirectX::XMFLOAT3 value;
		DirectX::XMStoreFloat3(&value, worldCorner);

		worldAABB.min.x = (std::min)(worldAABB.min.x, value.x);
		worldAABB.min.y = (std::min)(worldAABB.min.y, value.y);
		worldAABB.min.z = (std::min)(worldAABB.min.z, value.z);
		worldAABB.max.x = (std::max)(worldAABB.max.x, value.x);
		worldAABB.max.y = (std::max)(worldAABB.max.y, value.y);
		worldAABB.max.z = (std::max)(worldAABB.max.z, value.z);
	}

	return worldAABB;
}

bool Collider::Intersects(const ColliderAABB& lhs, const ColliderAABB& rhs)
{
	return lhs.min.x <= rhs.max.x && lhs.max.x >= rhs.min.x
		&& lhs.min.y <= rhs.max.y && lhs.max.y >= rhs.min.y
		&& lhs.min.z <= rhs.max.z && lhs.max.z >= rhs.min.z;
}

void Collider::CreateVertexShader()
{
	_vertexShader = std::make_shared<VertexShader>();
	_vertexShader->Create(L"src\\HLSL\\DefaultColor.hlsl", "VS", "vs_5_0");
}

void Collider::CreateInputLayout()
{
	_geometry = std::make_shared<Geometry>();
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	_geometry->Create(layout, _vertexShader->GetBlob());
}

void Collider::CreateVertexBuffer(const MeshData<VertexColorData>& meshData)
{
	_vertexBuffer = std::make_shared<VertexBuffer<VertexColorData>>();
	_vertexBuffer->Create(meshData.vertices);
}

void Collider::CreateIndexBuffer(const MeshData<VertexColorData>& meshData)
{
	_indexBuffer = std::make_shared<IndexBuffer>();
	_indexBuffer->Create(meshData.indices);
}

void Collider::CreatePixelShader()
{
	_pixelShader = std::make_shared<PixelShader>();
	_pixelShader->Create(L"src\\HLSL\\DefaultColor.hlsl", "PS", "ps_5_0");
}

void Collider::CreateConstantBuffer()
{
	_constantBuffer = std::make_shared<ConstantBuffer<TransformData>>();
	_constantBuffer->Create();
}
