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
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&_position);
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z);
	
	DirectX::XMVECTOR up = DirectX::XMVector3TransformNormal(DEFAULT_UP_VECTOR, rotMat);
	DirectX::XMVECTOR forward = DirectX::XMVector3TransformNormal(DEFAULT_FORWARD_VECTOR, rotMat);
	DirectX::XMVECTOR target = DirectX::XMVectorAdd(pos, forward);

	_viewMat = DirectX::XMMatrixLookAtLH(pos, target, up);
	_projMat = DirectX::XMMatrixPerspectiveFovLH(_fovAngle / 360.0f * DirectX::XM_2PI, Graphic::GetInstance()->GetWidth() / Graphic::GetInstance()->GetHeight(), 0.0001f, 1000.0f);
}
