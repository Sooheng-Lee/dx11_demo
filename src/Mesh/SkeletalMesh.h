#pragma once

class SkeletalMesh
{
public:
	SkeletalMesh() = default;
	~SkeletalMesh() = default;

	bool Init(
		const std::string& modelPath,
		const std::wstring& texturePath,
		const std::wstring& normalTexturePath = L"",
		const std::wstring& roughnessTexturePath = L"",
		const std::wstring& metallicTexturePath = L"");
	void Bind(ID3D11RasterizerState* rsState, ID3D11SamplerState* samplerState);
	void BindShadow(ID3D11RasterizerState* rsState, VertexShader* shadowVertexShader);
	void Draw();
	void SetNormalTextureEnabled(bool enabled) { _useNormalTexture = enabled; }
	bool IsNormalTextureEnabled() const { return _useNormalTexture; }
	void SetRoughnessTextureEnabled(bool enabled) { _useRoughnessTexture = enabled; }
	bool IsRoughnessTextureEnabled() const { return _useRoughnessTexture; }
	void SetMetallicTextureEnabled(bool enabled) { _useMetallicTexture = enabled; }
	bool IsMetallicTextureEnabled() const { return _useMetallicTexture; }

	const std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap() const { return _model.GetBoneInfoMap(); }
	UINT GetBoneCount() const { return _model.GetBoneCount(); }

private:
	void CreateVertexShader();
	void CreateInputLayout();
	void CreateVertexBuffer(const std::string& modelPath);
	void CreateIndexBuffer();
	void CreatePixelShader();
	void CreateSRV(
		const std::wstring& texturePath,
		const std::wstring& normalTexturePath,
		const std::wstring& roughnessTexturePath,
		const std::wstring& metallicTexturePath);

private:
	std::shared_ptr<Geometry> _geometry;
	std::shared_ptr<VertexBuffer<VertexAnimData>> _vertexBuffer;
	std::shared_ptr<IndexBuffer> _indexBuffer;
	std::shared_ptr<VertexShader> _vertexShader;
	std::shared_ptr<PixelShader> _pixelShader;
	std::shared_ptr<Texture> _texture;
	std::shared_ptr<Texture> _normalTexture;
	std::shared_ptr<Texture> _roughnessTexture;
	std::shared_ptr<Texture> _metallicTexture;
	Model<VertexAnimData> _model;
	bool _useNormalTexture = false;
	bool _useRoughnessTexture = false;
	bool _useMetallicTexture = false;
};
