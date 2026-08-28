#include "..\pch.h"
#include "Geometry.h"

Geometry::Geometry()
{
}

Geometry::~Geometry()
{
}

void Geometry::Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, const ComPtr<ID3DBlob>& blob)
{
	_layout = layout;
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateInputLayout(_layout.data(), _layout.size(),
		blob->GetBufferPointer(), blob->GetBufferSize(), _inputLayout.GetAddressOf());
	CHECK(hr);
}
