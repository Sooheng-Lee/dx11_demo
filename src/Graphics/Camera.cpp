#include "..\pch.h"
#include "Camera.h"

Camera::Camera()
{
	_transformMat = DirectX::XMMatrixIdentity();
	_viewMat = DirectX::XMMatrixIdentity();
	_projMat = DirectX::XMMatrixIdentity();
	_fovAngle = 90.0f;
	UpdateMatrix();
}

Camera::~Camera()
{
}

void Camera::SetPosition(FLOAT x, FLOAT y, FLOAT z)
{
	_position = DirectX::XMFLOAT3(x, y, z);
	UpdateMatrix();
}

void Camera::UpdateMatrix()
{
	_transformMat = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
	_transformMat *= DirectX::XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z);
	_transformMat *= DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z);

	DirectX::XMFLOAT4 at = DirectX::XMFLOAT4(_position.x + DEFAULT_FORWARD_VECTOR.x, _position.y + DEFAULT_FORWARD_VECTOR.y, _position.z + DEFAULT_FORWARD_VECTOR.z, 1.0f);
	DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&_position);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat4(&DEFAULT_UP_VECTOR);
	DirectX::XMVECTOR atVector = DirectX::XMLoadFloat4(&at);
	_viewMat = DirectX::XMMatrixLookAtLH(eye, atVector, up);
	_projMat = DirectX::XMMatrixPerspectiveFovLH(_fovAngle / 360.0f * DirectX::XM_2PI, Graphic::GetInstance()->GetWidth() / Graphic::GetInstance()->GetHeight(), 0.0001f, 1000.0f);
}
