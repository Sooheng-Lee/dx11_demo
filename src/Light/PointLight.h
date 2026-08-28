#pragma once

class PointLight
{
public:
	PointLight();
	~PointLight() = default;

	void SetPosition(float x, float y, float z);
	void SetColor(float r, float g, float b);
	void SetIntensity(float intensity);
	void SetRange(float range);
	void SetAttenuation(float constant, float linear, float quadratic);

	const DirectX::XMFLOAT3& GetPosition() const { return _position; }
	const DirectX::XMFLOAT3& GetColor() const { return _color; }
	const DirectX::XMFLOAT3& GetAttenuation() const { return _attenuation; }
	float GetIntensity() const { return _intensity; }
	float GetRange() const { return _range; }

private:
	DirectX::XMFLOAT3 _position = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	DirectX::XMFLOAT3 _color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT3 _attenuation = DirectX::XMFLOAT3(1.0f, 0.09f, 0.032f);
	float _intensity = 1.0f;
	float _range = 10.0f;
};
