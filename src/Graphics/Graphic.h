#pragma once
class Graphic
{
public:
	static std::shared_ptr<Graphic> GetInstance();
	void RenderBegin();
	void RenderEnd();
	bool Create(HWND handle, UINT width, UINT height);
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
	bool CreateDeviceAndSwapChain(HWND hwnd, UINT width, UINT height);
	bool CreateRenderTargetView(UINT width, UINT height);

private:
	static std::shared_ptr<Graphic> _graphic;

	std::vector<ComPtr<IDXGIAdapter>> _adapters;
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _deviceContext;
	ComPtr<IDXGISwapChain> _swapChain;

	ComPtr<ID3D11RenderTargetView> _renderTargetView;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;

	HWND _handle = nullptr;
	UINT _width = 0;
	UINT _height = 0;
};

