#pragma once

class StaticMesh
{
public:
	StaticMesh() = default;
	~StaticMesh() = default;

	bool Init(const std::string& modelPath, const std::wstring& texturePath);
	void Bind(ID3D11RasterizerState* rsState, ID3D11SamplerState* samplerState);
	void Draw();

private:
	void CreateVertexShader();
	void CreateInputLayout();
	void CreateVertexBuffer(const std::string& modelPath);
	void CreateIndexBuffer();
	void CreatePixelShader();
	void CreateSRV(const std::wstring& texturePath);

private:
	std::shared_ptr<Geometry> _geometry;
	std::shared_ptr<VertexBuffer<VertexTexData>> _vertexBuffer;
	std::shared_ptr<IndexBuffer> _indexBuffer;
	std::shared_ptr<VertexShader> _vertexShader;
	std::shared_ptr<PixelShader> _pixelShader;
	std::shared_ptr<Texture> _texture;
	Model<VertexTexData> _model;
};
