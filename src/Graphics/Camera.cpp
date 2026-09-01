#include "..\pch.h"
#include "Camera.h"

Camera::Camera()
{
	_transformMat = DirectX::XMMatrixIdentity();
	_viewMat = DirectX::XMMatrixIdentity();
	_projMat = DirectX::XMMatrixIdentity();
	_fovAngle = 75.0f;
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

void Camera::LookAt(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target)
{
	_position = position;
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&_position);
	DirectX::XMVECTOR targetVector = DirectX::XMLoadFloat3(&target);

	_viewMat = DirectX::XMMatrixLookAtLH(pos, targetVector, DEFAULT_UP_VECTOR);
	const FLOAT aspectRatio = static_cast<FLOAT>(Graphic::GetInstance()->GetWidth()) / static_cast<FLOAT>(Graphic::GetInstance()->GetHeight());
	_projMat = DirectX::XMMatrixPerspectiveFovLH(_fovAngle / 360.0f * DirectX::XM_2PI, aspectRatio, 0.1f, 1000.0f);
}

void Camera::UpdateMatrix()
{
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&_position);
	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z);
	
	DirectX::XMVECTOR up = DirectX::XMVector3TransformNormal(DEFAULT_UP_VECTOR, rotMat);
	DirectX::XMVECTOR forward = DirectX::XMVector3TransformNormal(DEFAULT_FORWARD_VECTOR, rotMat);
	DirectX::XMVECTOR target = DirectX::XMVectorAdd(pos, forward);

	_viewMat = DirectX::XMMatrixLookAtLH(pos, target, up);
	const FLOAT aspectRatio = static_cast<FLOAT>(Graphic::GetInstance()->GetWidth()) / static_cast<FLOAT>(Graphic::GetInstance()->GetHeight());
	_projMat = DirectX::XMMatrixPerspectiveFovLH(_fovAngle / 360.0f * DirectX::XM_2PI, aspectRatio, 0.1f, 1000.0f);
}
