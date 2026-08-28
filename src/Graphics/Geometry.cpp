#include "..\pch.h"
#include "Geometry.h"

Geometry::Geometry(const wchar_t* fileName)
{
	ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3DCompileFromFile(fileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0", 0, 0, _blob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
	}
}

Geometry::~Geometry()
{
}

void Geometry::Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout)
{
	_layout = layout;
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateInputLayout(_layout.data(), _layout.size(),
		_blob->GetBufferPointer(), _blob->GetBufferSize(), _inputLayout.GetAddressOf());
	CHECK(hr);
}
