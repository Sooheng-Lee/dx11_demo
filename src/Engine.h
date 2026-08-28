#pragma once
#include "Client.h"
class Engine : public Client
{
public:
	Engine();
	~Engine();
	bool Init();

protected:
	virtual void Update() override;
	virtual void Render() override;

private:
	bool GetAdapters();
	bool CreateDeviceAndSwapChain();
	bool CreateRenderTargetView();

	void CreateInputLayout();
	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateVertexShader();
	void CreatePixelShader();
	
	void RenderBegin();
	void RenderEnd();
private:
	// Graphics
	std::vector<ComPtr<IDXGIAdapter>> _adapters;
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _deviceContext;
	ComPtr<IDXGISwapChain> _swapChain;
	ComPtr<ID3D11RenderTargetView> _renderTargetView;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;

	// Geometry
	ComPtr<ID3D11InputLayout> _inputLayout;
	ComPtr<ID3DBlob> _vsBlob;

	// VS
	ComPtr<ID3D11Buffer> _vertexBuffer;
	ComPtr<ID3D11VertexShader> _vertexShader;
	ComPtr<ID3D11Buffer> _indexBuffer;

	// PS
	ComPtr<ID3DBlob> _psBlob;
	ComPtr<ID3D11PixelShader> _pixelShader;
};

