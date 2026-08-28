#include "pch.h"

Engine::Engine()
{
	_constData.worldMat = DirectX::XMMatrixIdentity();
	_constData.viewMat = DirectX::XMMatrixIdentity();
	_constData.projMat = DirectX::XMMatrixIdentity();
}

Engine::~Engine()
{
}

bool Engine::Init()
{
	if(!Graphic::GetInstance()->Create(_handle, _width, _height)) return false;
	
	CreateVertexShader();
	CreateInputLayout();
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreatePixelShader();
	CreateSRV();
	CreateConstantBuffer();
	CreateRSState();
	_camera = std::make_shared<Camera>();
	return true;
}

void Engine::Update()
{
	__super::Update();
	DirectX::XMFLOAT3 camPosition = _camera->GetPosition();
	if (_keyboard->GetKey('W'))
	{
		camPosition.z += 0.001f;
		OutputDebugStringW(L"W\n");
	}
	if (_keyboard->GetKey('S'))
	{
		camPosition.z -= 0.001f;
		OutputDebugStringW(L"S\n");
	}
	if (_keyboard->GetKey('D'))
	{
		camPosition.x += 0.001f;
		OutputDebugStringW(L"D\n");
	}
	if (_keyboard->GetKey('A'))
	{
		camPosition.x -= 0.001f;
		OutputDebugStringW(L"A\n");
	}
	_camera->SetPosition(camPosition.x, camPosition.y, camPosition.z);
	static float angle = 0.001f;
	angle += 0.0001f;
	_constData.worldMat = DirectX::XMMatrixRotationZ(angle) * _camera->GetViewMat() * _camera->GetProjMat();
	_constantBuffer->Update(&_constData);
}

void Engine::Render()
{
	Graphic::GetInstance()->RenderBegin();
	UINT offset = 0;
	Graphic::GetInstance()->GetDeviceContext()->IASetInputLayout(_geometry->GetComPtr().Get());
	Graphic::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), _vertexBuffer->GetStridePtr(), &offset);
	Graphic::GetInstance()->GetDeviceContext()->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, offset);
	Graphic::GetInstance()->GetDeviceContext()->RSSetState(_rsState.Get());
	Graphic::GetInstance()->GetDeviceContext()->VSSetShader(_vertexShader->GetComPtr().Get(), nullptr, 0);
	Graphic::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(0, 1, _constantBuffer->GetComPtr().GetAddressOf());
	Graphic::GetInstance()->GetDeviceContext()->PSSetShader(_pixelShader->GetComPtr().Get(), nullptr, 0);
	Graphic::GetInstance()->GetDeviceContext()->PSSetSamplers(0, 1, _samplerState.GetAddressOf());
	Graphic::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, _texture->GetComPtr().GetAddressOf());

	FLOAT blendFactor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	Graphic::GetInstance()->GetDeviceContext()->OMSetBlendState(_blendState.Get(), blendFactor, 0xFFFFFFFF);
	Graphic::GetInstance()->GetDeviceContext()->DrawIndexed(_indexBuffer->GetCount(), 0, 0);
	// RenderEnd
	Graphic::GetInstance()->RenderEnd();
}

void Engine::CreateInputLayout()
{
	_geometry = std::make_shared<Geometry>();
	std::vector<D3D11_INPUT_ELEMENT_DESC>layout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	_geometry->Create(layout, _vertexShader->GetBlob());
}

void Engine::CreateVertexBuffer()
{
	_vertexBuffer = std::make_shared<VertexBuffer<VertexTexData>>();
	std::vector<VertexTexData> vertices;
	vertices.resize(4);
	{
		vertices[0].position = DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f);
		vertices[0].uv = DirectX::XMFLOAT2(0.0f, 1.0f);
		vertices[1].position = DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f);
		vertices[1].uv = DirectX::XMFLOAT2(0.0f, 0.0f);
		vertices[2].position = DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f);
		vertices[2].uv = DirectX::XMFLOAT2(1.0f, 0.0f);
		vertices[3].position = DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f);
		vertices[3].uv = DirectX::XMFLOAT2(1.0f, 1.0f);
	}
	_vertexBuffer->Create(vertices);
}

void Engine::CreateIndexBuffer()
{
	_indexBuffer = std::make_shared<IndexBuffer>();
	std::vector<UINT> indices = { 0, 1, 2, 2, 3, 0 };
	_indexBuffer->Create(indices);
}

void Engine::CreateVertexShader()
{
	_vertexShader = std::make_shared<VertexShader>();
	_vertexShader->Create(L"src\\HLSL\\DefaultTexture.hlsl", "VS", "vs_5_0");
}

void Engine::CreatePixelShader()
{
	_pixelShader = std::make_shared<PixelShader>();
	_pixelShader->Create(L"src\\HLSL\\DefaultTexture.hlsl", "PS", "ps_5_0");
}

void Engine::CreateSRV()
{
	
	_texture = std::make_shared<Texture>();
	_texture->Create(L"Data\\Texture\\megaman.png");
}

void Engine::CreateConstantBuffer()
{
	_constantBuffer = std::make_shared<ConstantBuffer<TransformData>>();
	_constantBuffer->Create();
}

void Engine::CreateRSState()
{
	CD3D11_RASTERIZER_DESC desc{ D3D11_DEFAULT };
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateRasterizerState(&desc, _rsState.GetAddressOf());
	CHECK(hr);
}

void Engine::CreateBlendState()
{
	CD3D11_BLEND_DESC desc{ D3D11_DEFAULT };
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateBlendState(&desc, _blendState.GetAddressOf());
	CHECK(hr);
}

void Engine::CreateSamplerState()
{
	CD3D11_SAMPLER_DESC desc{ D3D11_DEFAULT };
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateSamplerState(&desc, _samplerState.GetAddressOf());
	CHECK(hr);
}