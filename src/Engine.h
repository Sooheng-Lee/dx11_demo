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
	void CreateInputLayout();
	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateVertexShader();
	void CreatePixelShader();
	void CreateSRV();
	void CreateConstantBuffer();

	void CreateRSState();
	void CreateBlendState();
	void CreateSamplerState();

private:
	// Graphics

	// Geometry
	std::shared_ptr<Geometry> _geometry;
	std::shared_ptr<VertexBuffer<VertexTexData>> _vertexBuffer;
	std::shared_ptr<IndexBuffer> _indexBuffer;
	std::shared_ptr<VertexShader> _vertexShader;
	std::shared_ptr<PixelShader> _pixelShader;
	std::shared_ptr<Texture> _texture;
	std::shared_ptr<ConstantBuffer<TransformData>> _constantBuffer;

	// States
	ComPtr<ID3D11RasterizerState> _rsState;
	ComPtr<ID3D11BlendState> _blendState;
	ComPtr<ID3D11SamplerState> _samplerState;

	TransformData _constData;
};

