#pragma once
class IndexBuffer
{
public:
	IndexBuffer();
	~IndexBuffer();
	void Create(const std::vector<UINT>& indices);
	ComPtr<ID3D11Buffer> GetComPtr() { return _buffer; };
	UINT GetCount() { return _count; };

private:
	UINT _count = 0;
	ComPtr<ID3D11Buffer> _buffer;
};

