#include "..\pch.h"
#include "Texture.h"
#include "WICTextureLoader.h"
#include "DirectXHelpers.h"

Texture::Texture()
{
}

Texture::~Texture()
{
}

void Texture::Create(const wchar_t* imgPath)
{
	HRESULT hr = DirectX::CreateWICTextureFromFile(
		Graphic::GetInstance()->GetDevice().Get(),
		Graphic::GetInstance()->GetDeviceContext().Get(),
		imgPath,
		nullptr,
		_shaderResourceView.GetAddressOf()
	);
	CHECK(hr);
}
