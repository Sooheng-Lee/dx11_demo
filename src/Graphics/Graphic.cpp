#include "..\pch.h"
#include "Graphic.h"

std::shared_ptr<Graphic> Graphic::_graphic = nullptr;

std::shared_ptr<Graphic> Graphic::GetInstance()
{
	if (_graphic == nullptr)
		_graphic = std::make_shared<Graphic>();
	return _graphic;
}

void Graphic::RenderBegin()
{
	FLOAT clearView[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = static_cast<FLOAT>(_width);
	viewport.Height = static_cast<FLOAT>(_height);
	_deviceContext->RSSetViewports(1, &viewport);
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), clearView);
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Graphic::RenderEnd()
{
	_swapChain->Present(0, 0);
}

bool Graphic::Create(HWND handle, UINT width, UINT height)
{
	_handle = handle;
	_width = width;
	_height = height;
	if (!CreateDeviceAndSwapChain(handle, width, height)) return false;
	if (!CreateRenderTargetView(width, height)) return false;
	return true;
}

bool Graphic::GetAdapters()
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

bool Graphic::CreateDeviceAndSwapChain(HWND hwnd, UINT width, UINT height)
{
	if (!GetAdapters()) return false;
	DXGI_SWAP_CHAIN_DESC desc = {};
	{
		desc.BufferCount = 1;
		desc.BufferDesc.Width = width;
		desc.BufferDesc.Height = height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Windowed = true;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.OutputWindow = hwnd;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}
	HRESULT hr = D3D11CreateDeviceAndSwapChain(_adapters[0].Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
		&desc, _swapChain.GetAddressOf(), _device.GetAddressOf(), nullptr, _deviceContext.GetAddressOf());
	if (FAILED(hr)) return false;
	return true;
}

bool Graphic::CreateRenderTargetView(UINT width, UINT height)
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
		desc.Width = width;
		desc.Height = height;
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
	viewport.MaxDepth = 1.0f;
	viewport.Width = static_cast<FLOAT>(width);
	viewport.Height = static_cast<FLOAT>(height);
	Graphic::GetInstance()->GetDeviceContext()->RSSetViewports(1, &viewport);
	return true;
}
