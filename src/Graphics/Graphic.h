#pragma once
class Graphic
{
public:
	static std::shared_ptr<Graphic> GetInstance();
	bool CreateDeviceAndSwapChain(HWND hwnd, UINT width, UINT height);
	ComPtr<ID3D11Device> GetDevice() {
		return _device;
	};

	ComPtr<ID3D11DeviceContext> GetDeviceContext() {
		return _deviceContext;
	}

	ComPtr<IDXGISwapChain> GetSwapChain() {
		return _swapChain;
	}

private:
	bool GetAdapters();

private:
	static std::shared_ptr<Graphic> _graphic;

	std::vector<ComPtr<IDXGIAdapter>> _adapters;
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _deviceContext;
	ComPtr<IDXGISwapChain> _swapChain;
};

