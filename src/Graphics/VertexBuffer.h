#pragma once

template <typename T>
class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();
	void Create(const std::vector<T>& vertices);
	ComPtr<ID3D11Buffer> GetComPtr() { return _buffer; };
	const UINT* GetStridePtr() { return &_stride; };
	const UINT GetStride() { return _stride; };
	const UINT GetCount() { return _count; };

private:
	ComPtr<ID3D11Buffer> _buffer;
	UINT _stride = 0;
	UINT _count = 0;
};

template<typename T>
inline VertexBuffer<T>::VertexBuffer()
{
	_stride = sizeof(T);
}

template<typename T>
inline VertexBuffer<T>::~VertexBuffer()
{
}

template<typename T>
void VertexBuffer<T>::Create(const std::vector<T>& vertices)
{
	_count = vertices.size();
	D3D11_BUFFER_DESC desc = {};
	{
		desc.ByteWidth = _stride * _count;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.MiscFlags = 0;
	}

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = vertices.data();
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateBuffer(&desc, &data, _buffer.GetAddressOf());
	
	CHECK(hr);
}