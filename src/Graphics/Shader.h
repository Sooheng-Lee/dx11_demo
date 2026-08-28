#pragma once
class Shader
{
public:
	Shader();
	~Shader();
	ComPtr<ID3DBlob> GetBlob() { return _blob; };
	virtual void Create(const wchar_t* filePath, const char* entryPoint, const char* target);
protected:
	ComPtr<ID3DBlob> _blob;
};

class VertexShader : public Shader
{
	using Super = Shader;
public:
	VertexShader();
	~VertexShader();
	void Create(const wchar_t* filePath, const char* entryPoint, const char* target) override;
	ComPtr<ID3D11VertexShader> GetComPtr() { return _shader; };
private:
	ComPtr<ID3D11VertexShader> _shader;
};

class PixelShader : public Shader
{
	using Super = Shader;
public:
	PixelShader();
	~PixelShader();
	void Create(const wchar_t* filePath, const char* entryPoint, const char* target) override;
	ComPtr<ID3D11PixelShader> GetComPtr() { return _shader; };

private:
	ComPtr<ID3D11PixelShader> _shader;
};