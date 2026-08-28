#pragma once
class Geometry
{
public:
	Geometry();
	~Geometry();

	ComPtr<ID3D11InputLayout> GetComPtr() {
		return _inputLayout;
	}
	void Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, const ComPtr<ID3DBlob>& blob);

private:
	ComPtr<ID3D11InputLayout> _inputLayout;
	std::vector<D3D11_INPUT_ELEMENT_DESC> _layout;
};

