#include "..\pch.h"
#include "SkeletalMesh.h"

bool SkeletalMesh::Init(
	const std::string& modelPath,
	const std::wstring& texturePath,
	const std::wstring& normalTexturePath,
	const std::wstring& roughnessTexturePath,
	const std::wstring& metallicTexturePath)
{
	CreateVertexShader();
	CreateInputLayout();
	CreateVertexBuffer(modelPath);
	CreateIndexBuffer();
	CreatePixelShader();
	CreateSRV(texturePath, normalTexturePath, roughnessTexturePath, metallicTexturePath);

	return true;
}

void SkeletalMesh::Bind(ID3D11RasterizerState* rsState, ID3D11SamplerState* samplerState)
{
	UINT offset = 0;
	Graphic::GetInstance()->GetDeviceContext()->IASetInputLayout(_geometry->GetComPtr().Get());
	Graphic::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), _vertexBuffer->GetStridePtr(), &offset);
	Graphic::GetInstance()->GetDeviceContext()->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, offset);
	Graphic::GetInstance()->GetDeviceContext()->RSSetState(rsState);
	Graphic::GetInstance()->GetDeviceContext()->VSSetShader(_vertexShader->GetComPtr().Get(), nullptr, 0);
	Graphic::GetInstance()->GetDeviceContext()->PSSetShader(_pixelShader->GetComPtr().Get(), nullptr, 0);
	Graphic::GetInstance()->GetDeviceContext()->PSSetSamplers(0, 1, &samplerState);
	Graphic::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, _texture->GetComPtr().GetAddressOf());
	ID3D11ShaderResourceView* normalTextureView = _normalTexture != nullptr ? _normalTexture->GetComPtr().Get() : nullptr;
	Graphic::GetInstance()->GetDeviceContext()->PSSetShaderResources(1, 1, &normalTextureView);
	ID3D11ShaderResourceView* roughnessTextureView = _roughnessTexture != nullptr ? _roughnessTexture->GetComPtr().Get() : nullptr;
	Graphic::GetInstance()->GetDeviceContext()->PSSetShaderResources(2, 1, &roughnessTextureView);
	ID3D11ShaderResourceView* metallicTextureView = _metallicTexture != nullptr ? _metallicTexture->GetComPtr().Get() : nullptr;
	Graphic::GetInstance()->GetDeviceContext()->PSSetShaderResources(3, 1, &metallicTextureView);
}

void SkeletalMesh::BindShadow(ID3D11RasterizerState* rsState, VertexShader* shadowVertexShader)
{
	UINT offset = 0;
	Graphic::GetInstance()->GetDeviceContext()->IASetInputLayout(_geometry->GetComPtr().Get());
	Graphic::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), _vertexBuffer->GetStridePtr(), &offset);
	Graphic::GetInstance()->GetDeviceContext()->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, offset);
	Graphic::GetInstance()->GetDeviceContext()->RSSetState(rsState);
	Graphic::GetInstance()->GetDeviceContext()->VSSetShader(shadowVertexShader->GetComPtr().Get(), nullptr, 0);
	Graphic::GetInstance()->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);
}

void SkeletalMesh::Draw()
{
	Graphic::GetInstance()->GetDeviceContext()->DrawIndexed(_indexBuffer->GetCount(), 0, 0);
}

void SkeletalMesh::CreateVertexShader()
{
	_vertexShader = std::make_shared<VertexShader>();
	_vertexShader->Create(L"src\\HLSL\\DefaultAnim.hlsl", "VS", "vs_5_0");
}

void SkeletalMesh::CreateInputLayout()
{
	_geometry = std::make_shared<Geometry>();
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	_geometry->Create(layout, _vertexShader->GetBlob());
}

void SkeletalMesh::CreateVertexBuffer(const std::string& modelPath)
{
	_model.Load(modelPath);
	_vertexBuffer = std::make_shared<VertexBuffer<VertexAnimData>>();
	_vertexBuffer->Create(_model.GetVertices());
}

void SkeletalMesh::CreateIndexBuffer()
{
	_indexBuffer = std::make_shared<IndexBuffer>();
	_indexBuffer->Create(_model.GetIndices());
}

void SkeletalMesh::CreatePixelShader()
{
	_pixelShader = std::make_shared<PixelShader>();
	_pixelShader->Create(L"src\\HLSL\\DefaultAnim.hlsl", "PS", "ps_5_0");
}

void SkeletalMesh::CreateSRV(
	const std::wstring& texturePath,
	const std::wstring& normalTexturePath,
	const std::wstring& roughnessTexturePath,
	const std::wstring& metallicTexturePath)
{
	_texture = std::make_shared<Texture>();
	_texture->Create(texturePath.c_str());

	_useNormalTexture = !normalTexturePath.empty();
	if (_useNormalTexture)
	{
		_normalTexture = std::make_shared<Texture>();
		_normalTexture->Create(normalTexturePath.c_str());
	}

	_useRoughnessTexture = !roughnessTexturePath.empty();
	if (_useRoughnessTexture)
	{
		_roughnessTexture = std::make_shared<Texture>();
		_roughnessTexture->Create(roughnessTexturePath.c_str());
	}

	_useMetallicTexture = !metallicTexturePath.empty();
	if (_useMetallicTexture)
	{
		_metallicTexture = std::make_shared<Texture>();
		_metallicTexture->Create(metallicTexturePath.c_str());
	}
}
