#pragma once
template <typename T>
class ConstantBuffer
{
public:
	ConstantBuffer() {};
	~ConstantBuffer() {};

	void Update(T* data)
	{
		D3D11_MAPPED_SUBRESOURCE subResource = {};
		HRESULT hr = Graphic::GetInstance()->GetDeviceContext()->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		if (SUCCEEDED(hr))
		{
			memcpy(subResource.pData, reinterpret_cast<void*>(data), sizeof(T));
			Graphic::GetInstance()->GetDeviceContext()->Unmap(_buffer.Get(), 0);
		}
	};

	void Create() {
		D3D11_BUFFER_DESC desc = {};
		{
			desc.ByteWidth = sizeof(T);
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
		}
		Graphic::GetInstance()->GetDevice()->CreateBuffer(&desc, nullptr, _buffer.GetAddressOf());
	};

	ComPtr<ID3D11Buffer> GetComPtr() { return _buffer; };
private:
	ComPtr<ID3D11Buffer> _buffer;
};