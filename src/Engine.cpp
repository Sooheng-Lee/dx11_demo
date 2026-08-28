#include "pch.h"
#include "WICTextureLoader.h"
#include "DirectXHelpers.h"
Engine::Engine()
{
}

Engine::~Engine()
{
}

bool Engine::Init()
{
	if(!Graphic::GetInstance()->CreateDeviceAndSwapChain(_handle, _width, _height)) return false;
	if (!CreateRenderTargetView()) return false;

	CreateInputLayout();
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateVertexShader();
	CreatePixelShader();
	CreateSRV();

	CreateRSState();
	return true;
}

void Engine::Update()
{
}

void Engine::Render()
{
	RenderBegin();
	UINT stride = sizeof(VertexTexData);
	UINT offset = 0;
	Graphic::GetInstance()->GetDeviceContext()->IASetInputLayout(geometry->GetComPtr().Get());
	Graphic::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	Graphic::GetInstance()->GetDeviceContext()->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, offset);
	Graphic::GetInstance()->GetDeviceContext()->RSSetState(_rsState.Get());
	Graphic::GetInstance()->GetDeviceContext()->VSSetShader(_vertexShader.Get(), nullptr, 0);
	Graphic::GetInstance()->GetDeviceContext()->PSSetShader(_pixelShader.Get(), nullptr, 0);
	Graphic::GetInstance()->GetDeviceContext()->PSSetSamplers(0, 1, _samplerState.GetAddressOf());
	Graphic::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, _shaderResourceView.GetAddressOf());
	FLOAT blendFactor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	Graphic::GetInstance()->GetDeviceContext()->OMSetBlendState(_blendState.Get(), blendFactor, 0xFFFFFFFF);
	RenderEnd();
}

bool Engine::CreateRenderTargetView()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = Graphic::GetInstance()->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr)) return false;
	hr = Graphic::GetInstance()->GetDevice()->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
	if (FAILED(hr)) return false;

	ComPtr<ID3D11Texture2D> depthBuffer;
	D3D11_TEXTURE2D_DESC desc = {};
	{
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ArraySize = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.Width = _width;
		desc.Height = _height;
		desc.MipLevels = 1;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
	}
	hr = Graphic::GetInstance()->GetDevice()->CreateTexture2D(&desc, nullptr, depthBuffer.GetAddressOf());
	if (FAILED(hr)) return false;
	hr = Graphic::GetInstance()->GetDevice()->CreateDepthStencilView(depthBuffer.Get(), nullptr, _depthStencilView.GetAddressOf());
	if (FAILED(hr)) return false;
	Graphic::GetInstance()->GetDeviceContext()->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 0.0f;
	viewport.Width = static_cast<FLOAT>(_width);
	viewport.Height = static_cast<FLOAT>(_height);
	Graphic::GetInstance()->GetDeviceContext()->RSSetViewports(1, &viewport);
	return true;
}

void Engine::CreateInputLayout()
{
	geometry = std::make_shared<Geometry>(L"src\\HLSL\\DefaultTexture.hlsl");
	std::vector<D3D11_INPUT_ELEMENT_DESC>layout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	geometry->Create(layout);
}

void Engine::CreateVertexBuffer()
{
	/*
	std::vector<VertexColorData> vertices;
	vertices.resize(4);
	{
		vertices[0].position = DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f);
		vertices[0].color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		vertices[1].position = DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f);
		vertices[1].color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		vertices[2].position = DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f);
		vertices[2].color = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		vertices[3].position = DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f);
		vertices[3].color = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	}
	*/

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
	D3D11_BUFFER_DESC desc = {};
	{
		desc.ByteWidth = sizeof(VertexTexData) * vertices.size();
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.MiscFlags = 0;
	}
	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = vertices.data();
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
	CHECK(hr);
}

void Engine::CreateIndexBuffer()
{
	std::vector<UINT> indices = { 0, 1, 2, 2, 3, 0 };
	D3D11_BUFFER_DESC desc = {};
	{
		desc.ByteWidth = sizeof(UINT) * indices.size();
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.MiscFlags = 0;
	}
	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = indices.data();
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
	CHECK(hr);
}

void Engine::CreateVertexShader()
{
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateVertexShader(geometry->GetBlob()->GetBufferPointer(), geometry->GetBlob()->GetBufferSize(), nullptr, _vertexShader.GetAddressOf());
	CHECK(hr);
}

void Engine::CreatePixelShader()
{
	ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3DCompileFromFile(L"src\\HLSL\\DefaultTexture.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", 0, 0, _psBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (errorBlob)
	{
		OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
		return;
	}
	hr = Graphic::GetInstance()->GetDevice()->CreatePixelShader(_psBlob->GetBufferPointer(), _psBlob->GetBufferSize(), nullptr, _pixelShader.GetAddressOf());
	CHECK(hr);
}

void Engine::CreateSRV()
{
	HRESULT hr = DirectX::CreateWICTextureFromFile(
		Graphic::GetInstance()->GetDevice().Get(),
		Graphic::GetInstance()->GetDeviceContext().Get(),
		L"Data\\Texture\\megaman.png",
		nullptr,
		_shaderResourceView.GetAddressOf()
	);
	CHECK(hr);
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

void Engine::RenderBegin()
{
	FLOAT clearView[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	Graphic::GetInstance()->GetDeviceContext()->ClearRenderTargetView(_renderTargetView.Get(), clearView);
	Graphic::GetInstance()->GetDeviceContext()->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	Graphic::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Engine::RenderEnd()
{
	Graphic::GetInstance()->GetDeviceContext()->DrawIndexed(6, 0, 0);
	Graphic::GetInstance()->GetSwapChain()->Present(0, 0);
}
