#include "..\pch.h"

PointLight::PointLight()
{
}

void PointLight::SetPosition(float x, float y, float z)
{
	_position = DirectX::XMFLOAT3(x, y, z);
}

void PointLight::SetColor(float r, float g, float b)
{
	_color = DirectX::XMFLOAT3(r, g, b);
}

void PointLight::SetIntensity(float intensity)
{
	_intensity = (std::max)(0.0f, intensity);
}

void PointLight::SetRange(float range)
{
	_range = (std::max)(0.0f, range);
}

void PointLight::SetAttenuation(float constant, float linear, float quadratic)
{
	_attenuation = DirectX::XMFLOAT3(
		(std::max)(0.0f, constant),
		(std::max)(0.0f, linear),
		(std::max)(0.0f, quadratic));
}
