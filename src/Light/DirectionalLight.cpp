#include "..\pch.h"

DirectionalLight::DirectionalLight()
{
	NormalizeDirection();
}

void DirectionalLight::SetDirection(float x, float y, float z)
{
	_direction = DirectX::XMFLOAT3(x, y, z);
	NormalizeDirection();
}

void DirectionalLight::SetColor(float r, float g, float b)
{
	_color = DirectX::XMFLOAT3(r, g, b);
}

void DirectionalLight::SetIntensity(float intensity)
{
	_intensity = (std::max)(0.0f, intensity);
}

void DirectionalLight::NormalizeDirection()
{
	DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&_direction);

	if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(direction)) <= 0.0001f)
	{
		_direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
		return;
	}

	direction = DirectX::XMVector3Normalize(direction);
	DirectX::XMStoreFloat3(&_direction, direction);
}
