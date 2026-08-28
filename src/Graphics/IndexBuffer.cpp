#include "..\pch.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer()
{
}

IndexBuffer::~IndexBuffer()
{
}

void IndexBuffer::Create(const std::vector<UINT>& indices)
{
	_count = static_cast<UINT>(indices.size());
	D3D11_BUFFER_DESC desc = {};
	{
		desc.ByteWidth = sizeof(UINT) * _count;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.MiscFlags = 0;
	}

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = indices.data();
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateBuffer(&desc, &data, _buffer.GetAddressOf());
	CHECK(hr);
}