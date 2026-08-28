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

	bool CreateRenderTargetView();

	void CreateInputLayout();
	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateVertexShader();
	void CreatePixelShader();
	void CreateSRV();

	void CreateRSState();
	void CreateBlendState();
	void CreateSamplerState();

	void RenderBegin();
	void RenderEnd();
private:
	// Graphics
	ComPtr<ID3D11RenderTargetView> _renderTargetView;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;

	// Geometry
	std::shared_ptr<Geometry> geometry;

	// VS
	ComPtr<ID3D11Buffer> _vertexBuffer;
	ComPtr<ID3D11VertexShader> _vertexShader;
	ComPtr<ID3D11Buffer> _indexBuffer;

	// PS
	ComPtr<ID3DBlob> _psBlob;
	ComPtr<ID3D11PixelShader> _pixelShader;
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView;

	// States
	ComPtr<ID3D11RasterizerState> _rsState;
	ComPtr<ID3D11BlendState> _blendState;
	ComPtr<ID3D11SamplerState> _samplerState;
};

