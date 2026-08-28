#pragma once
class Geometry
{
public:
	Geometry(const wchar_t* fileName);
	~Geometry();

	ComPtr<ID3D11InputLayout> GetComPtr() {
		return _inputLayout;
	}

	ComPtr<ID3DBlob> GetBlob() {
		return _blob;
	}

	void Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout);

private:
	ComPtr<ID3D11InputLayout> _inputLayout;
	ComPtr<ID3DBlob> _blob;
	std::vector<D3D11_INPUT_ELEMENT_DESC> _layout;
};

