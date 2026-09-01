#include "..\pch.h"

bool StaticMesh::Init(const std::string& modelPath, const std::wstring& texturePath)
{
	CreateVertexShader();
	CreateInputLayout();
	CreateVertexBuffer(modelPath);
	CreateIndexBuffer(_model.GetIndices());
	CreatePixelShader();
	CreateSRV(texturePath);
	CreateMaterialBuffer(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), true);

	return true;
}

bool StaticMesh::Init(const MeshData<VertexTexData>& meshData, const std::wstring& texturePath)
{
	CreateVertexShader();
	CreateInputLayout();
	CreateVertexBuffer(meshData);
	CreateIndexBuffer(meshData.indices);
	CreatePixelShader();
	CreateSRV(texturePath);
	CreateMaterialBuffer(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), true);

	return true;
}

bool StaticMesh::Init(const MeshData<VertexTexData>& meshData, const DirectX::XMFLOAT4& color)
{
	CreateVertexShader();
	CreateInputLayout();
	CreateVertexBuffer(meshData);
	CreateIndexBuffer(meshData.indices);
	CreatePixelShader();
	CreateMaterialBuffer(color, false);

	return true;
}

void StaticMesh::Bind(ID3D11RasterizerState* rsState, ID3D11SamplerState* samplerState)
{
	UINT offset = 0;
	Graphic::GetInstance()->GetDeviceContext()->IASetInputLayout(_geometry->GetComPtr().Get());
	Graphic::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), _vertexBuffer->GetStridePtr(), &offset);
	Graphic::GetInstance()->GetDeviceContext()->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, offset);
	Graphic::GetInstance()->GetDeviceContext()->RSSetState(rsState);
	Graphic::GetInstance()->GetDeviceContext()->VSSetShader(_vertexShader->GetComPtr().Get(), nullptr, 0);
	Graphic::GetInstance()->GetDeviceContext()->PSSetShader(_pixelShader->GetComPtr().Get(), nullptr, 0);
	Graphic::GetInstance()->GetDeviceContext()->PSSetSamplers(0, 1, &samplerState);
	if (_materialConstantBuffer != nullptr)
	{
		Graphic::GetInstance()->GetDeviceContext()->PSSetConstantBuffers(0, 1, _materialConstantBuffer->GetComPtr().GetAddressOf());
	}

	ID3D11ShaderResourceView* textureView = _texture != nullptr ? _texture->GetComPtr().Get() : nullptr;
	Graphic::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, &textureView);
}

void StaticMesh::Draw()
{
	Graphic::GetInstance()->GetDeviceContext()->DrawIndexed(_indexBuffer->GetCount(), 0, 0);
}

void StaticMesh::CreateVertexShader()
{
	_vertexShader = std::make_shared<VertexShader>();
	_vertexShader->Create(L"src\\HLSL\\DefaultTexture.hlsl", "VS", "vs_5_0");
}

void StaticMesh::CreateInputLayout()
{
	_geometry = std::make_shared<Geometry>();
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	_geometry->Create(layout, _vertexShader->GetBlob());
}

void StaticMesh::CreateVertexBuffer(const std::string& modelPath)
{
	_model.Load(modelPath);
	_vertexBuffer = std::make_shared<VertexBuffer<VertexTexData>>();
	_vertexBuffer->Create(_model.GetVertices());
}

void StaticMesh::CreateVertexBuffer(const MeshData<VertexTexData>& meshData)
{
	_vertexBuffer = std::make_shared<VertexBuffer<VertexTexData>>();
	_vertexBuffer->Create(meshData.vertices);
}

void StaticMesh::CreateIndexBuffer(const std::vector<UINT>& indices)
{
	_indexBuffer = std::make_shared<IndexBuffer>();
	_indexBuffer->Create(indices);
}

void StaticMesh::CreatePixelShader()
{
	_pixelShader = std::make_shared<PixelShader>();
	_pixelShader->Create(L"src\\HLSL\\DefaultTexture.hlsl", "PS", "ps_5_0");
}

void StaticMesh::CreateSRV(const std::wstring& texturePath)
{
	_texture = std::make_shared<Texture>();
	_texture->Create(texturePath.c_str());
}

void StaticMesh::CreateMaterialBuffer(const DirectX::XMFLOAT4& color, bool useTexture)
{
	_materialConstData.color = color;
	_materialConstData.useTexture = useTexture ? 1 : 0;
	_materialConstantBuffer = std::make_shared<ConstantBuffer<TextureMaterialData>>();
	_materialConstantBuffer->Create();
	_materialConstantBuffer->Update(&_materialConstData);
}
