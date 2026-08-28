#pragma once
class Texture
{
public:
	Texture();
	~Texture();
	void Create(const wchar_t* imgPath);
	const ComPtr<ID3D11ShaderResourceView>& GetComPtr(){ return _shaderResourceView;}
private:
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView;
};

