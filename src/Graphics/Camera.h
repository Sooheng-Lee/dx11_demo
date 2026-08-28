#pragma once

class Camera
{
public:
	Camera();
	~Camera();

public:
	void SetPosition(FLOAT x, FLOAT y, FLOAT z);
	DirectX::XMFLOAT3 GetPosition() { return _position; };
	DirectX::XMMATRIX GetViewMat() { return _viewMat; };
	DirectX::XMMATRIX GetProjMat() { return _projMat; };
	void UpdateMatrix();

private:
	DirectX::XMFLOAT3 _position = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
	DirectX::XMFLOAT3 _rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	DirectX::XMMATRIX _transformMat;
	DirectX::XMMATRIX _viewMat;
	DirectX::XMMATRIX _projMat;
	
	FLOAT _fovAngle = 0.0f;
	const DirectX::XMFLOAT4 DEFAULT_UP_VECTOR = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMFLOAT4 DEFAULT_FORWARD_VECTOR = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
};

