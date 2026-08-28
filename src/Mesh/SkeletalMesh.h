#pragma once

class SkeletalMesh
{
public:
	SkeletalMesh() = default;
	~SkeletalMesh() = default;

	bool Init(const std::string& modelPath, const std::wstring& texturePath);
	void Bind(ID3D11RasterizerState* rsState, ID3D11SamplerState* samplerState);
	void Draw();

	const std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap() const { return _model.GetBoneInfoMap(); }
	UINT GetBoneCount() const { return _model.GetBoneCount(); }

private:
	void CreateVertexShader();
	void CreateInputLayout();
	void CreateVertexBuffer(const std::string& modelPath);
	void CreateIndexBuffer();
	void CreatePixelShader();
	void CreateSRV(const std::wstring& texturePath);

private:
	std::shared_ptr<Geometry> _geometry;
	std::shared_ptr<VertexBuffer<VertexAnimData>> _vertexBuffer;
	std::shared_ptr<IndexBuffer> _indexBuffer;
	std::shared_ptr<VertexShader> _vertexShader;
	std::shared_ptr<PixelShader> _pixelShader;
	std::shared_ptr<Texture> _texture;
	Model<VertexAnimData> _model;
};
