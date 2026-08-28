#pragma once

class DirectionalLight
{
public:
	DirectionalLight();
	~DirectionalLight() = default;

	void SetDirection(float x, float y, float z);
	void SetColor(float r, float g, float b);
	void SetIntensity(float intensity);

	const DirectX::XMFLOAT3& GetDirection() const { return _direction; }
	const DirectX::XMFLOAT3& GetColor() const { return _color; }
	float GetIntensity() const { return _intensity; }

private:
	void NormalizeDirection();

private:
	DirectX::XMFLOAT3 _direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	DirectX::XMFLOAT3 _color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float _intensity = 1.0f;
};
