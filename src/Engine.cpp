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
	return true;
}

void Engine::Update()
{
}

void Engine::Render()
{
	RenderBegin();
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

void Engine::RenderBegin()
{
	FLOAT clearView[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), clearView);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Engine::RenderEnd()
{
	_swapChain->Present(0, 0);
}
