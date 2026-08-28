#include "pch.h"

Engine::Engine()
{
}

Engine::~Engine()
{
}

bool Engine::Init()
{
	if (!GetAdapters()) return false;
	if (!CreateDeviceAndSwapChain()) return false;
	if (!CreateRenderTargetView()) return false;

	CreateInputLayout();
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateVertexShader();
	CreatePixelShader();
	return true;
}

void Engine::Update()
{
}

void Engine::Render()
{
	RenderBegin();
	UINT stride = sizeof(VertexColorData);
	UINT offset = 0;
	_deviceContext->IASetInputLayout(_inputLayout.Get());
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, offset);
	_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
	_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);
	_deviceContext->DrawIndexed(6, 0, 0);
	RenderEnd();
}

bool Engine::GetAdapters()
{
	ComPtr<IDXGIFactory> factory;
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.GetAddressOf()));
	CHECK(hr);

	UINT adapterIdx = 0;
	ComPtr<IDXGIAdapter> adapter;
	while (SUCCEEDED(factory->EnumAdapters(adapterIdx, adapter.GetAddressOf())))
	{
		_adapters.push_back(adapter);
		DXGI_ADAPTER_DESC desc = {};
		adapter->GetDesc(&desc);
		adapterIdx++;
	}
	
	if (adapterIdx == 0) return false;
	return true;
}

bool Engine::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc = {};
	{
		desc.BufferCount = 1;
		desc.BufferDesc.Width = _width;
		desc.BufferDesc.Height = _height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Windowed = true;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.OutputWindow = _handle;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}
	HRESULT hr = D3D11CreateDeviceAndSwapChain(_adapters[0].Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
		&desc, _swapChain.GetAddressOf(), _device.GetAddressOf(), nullptr, _deviceContext.GetAddressOf());
	if (FAILED(hr)) return false;
	return true;
}

bool Engine::CreateRenderTargetView()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr)) return false;
	hr = _device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
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
	hr = _device->CreateTexture2D(&desc, nullptr, depthBuffer.GetAddressOf());
	if (FAILED(hr)) return false;
	hr = _device->CreateDepthStencilView(depthBuffer.Get(), nullptr, _depthStencilView.GetAddressOf());
	if (FAILED(hr)) return false;
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 0.0f;
	viewport.Width = static_cast<FLOAT>(_width);
	viewport.Height = static_cast<FLOAT>(_height);
	_deviceContext->RSSetViewports(1, &viewport);
	return true;
}

void Engine::CreateInputLayout()
{
	ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3DCompileFromFile(L"src\\HLSL\\DefaultColor.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0", 0, 0, _vsBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (errorBlob)
	{
		OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
		return;
	}

	D3D11_INPUT_ELEMENT_DESC layouts[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	
	hr = _device->CreateInputLayout(layouts, ARRAYSIZE(layouts), _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), _inputLayout.GetAddressOf());
	CHECK(hr);
}

void Engine::CreateVertexBuffer()
{
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
	D3D11_BUFFER_DESC desc = {};
	{
		desc.ByteWidth = sizeof(VertexColorData) * vertices.size();
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.MiscFlags = 0;
	}
	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = vertices.data();
	HRESULT hr = _device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
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
	HRESULT hr = _device->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
	CHECK(hr);
}

void Engine::CreateVertexShader()
{
	HRESULT hr = _device->CreateVertexShader(_vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), nullptr, _vertexShader.GetAddressOf());
	CHECK(hr);
}

void Engine::CreatePixelShader()
{
	ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3DCompileFromFile(L"src\\HLSL\\DefaultColor.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", 0, 0, _psBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (errorBlob)
	{
		OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
		return;
	}
	hr = _device->CreatePixelShader(_psBlob->GetBufferPointer(), _psBlob->GetBufferSize(), nullptr, _pixelShader.GetAddressOf());
	CHECK(hr);
}

void Engine::RenderBegin()
{
	FLOAT clearView[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), clearView);
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Engine::RenderEnd()
{
	_swapChain->Present(0, 0);
}
