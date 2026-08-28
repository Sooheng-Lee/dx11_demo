#include "..\pch.h"
#include "Shader.h"

Shader::Shader()
{
}

Shader::~Shader()
{
}

VertexShader::VertexShader()
{
}

VertexShader::~VertexShader()
{
}

void VertexShader::Create(const wchar_t* filePath, const char* entryPoint, const char* target)
{
	Super::Create(filePath, entryPoint, target);
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateVertexShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr, _shader.GetAddressOf());
	CHECK(hr);
}

void Shader::Create(const wchar_t* filePath, const char* entryPoint, const char* target)
{
	ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3DCompileFromFile(filePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, target, 0, 0, _blob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
	}
	CHECK(hr);
}

PixelShader::PixelShader()
{
}

PixelShader::~PixelShader()
{
}

void PixelShader::Create(const wchar_t* filePath, const char* entryPoint, const char* target)
{
	Super::Create(filePath, entryPoint, target);
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreatePixelShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr, _shader.GetAddressOf());
	CHECK(hr);
}
