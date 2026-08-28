#include "..\pch.h"
#include "Graphic.h"

std::shared_ptr<Graphic> Graphic::_graphic = nullptr;

std::shared_ptr<Graphic> Graphic::GetInstance()
{
	if (_graphic == nullptr)
		_graphic = std::make_shared<Graphic>();
	return _graphic;
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