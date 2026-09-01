#include "pch.h"
#include "EnemyObject.h"

bool EnemyObject::Init(const DirectX::XMFLOAT3& position)
{
	_position = position;
	_mesh = std::make_shared<StaticMesh>();
	_mesh->Init(PrimitiveGeometry::CreateCapsule(0.35f, 1.5f), _baseColor);

	_constantBuffer = std::make_shared<ConstantBuffer<TransformData>>();
	_constantBuffer->Create();

	std::shared_ptr<CapsuleCollider> collider = std::make_shared<CapsuleCollider>(0.35f, 1.5f);
	collider->Init();
	_collider = collider;

	return true;
}

void EnemyObject::Update(float deltaTime)
{
	if (_hitStunTime > 0.0f)
	{
		_hitStunTime = (std::max)(0.0f, _hitStunTime - deltaTime);
	}

	if (_attackCooldown > 0.0f)
	{
		_attackCooldown = (std::max)(0.0f, _attackCooldown - deltaTime);
	}

	if (_mesh != nullptr)
	{
		if (_hitStunTime > 0.0f)
		{
			const int flickerIndex = static_cast<int>((_hitStunDuration - _hitStunTime) * 10.0f);
			_mesh->SetColor((flickerIndex % 2) == 0 ? _hitColor : _baseColor);
		}
		else
		{
			_mesh->SetColor(_baseColor);
		}
	}

	const float lerpRatio = (std::min)(deltaTime * _tiltReturnSpeed, 1.0f);
	_tiltAmount += (_targetTiltAmount - _tiltAmount) * lerpRatio;
	_targetTiltAmount += (0.0f - _targetTiltAmount) * lerpRatio;
}

void EnemyObject::Render(
	const DirectX::XMMATRIX& viewMat,
	const DirectX::XMMATRIX& projMat,
	ID3D11RasterizerState* rsState,
	ID3D11SamplerState* samplerState,
	const std::shared_ptr<ConstantBuffer<LightData>>& lightConstantBuffer,
	const std::shared_ptr<ConstantBuffer<ShadowData>>& shadowConstantBuffer,
	ID3D11ShaderResourceView* shadowShaderResourceView,
	ID3D11SamplerState* shadowSamplerState)
{
	if (_mesh == nullptr || _constantBuffer == nullptr)
	{
		return;
	}

	TransformData transformData;
	transformData.worldMat = DirectX::XMMatrixTranspose(GetWorldMatrix());
	transformData.viewMat = DirectX::XMMatrixTranspose(viewMat);
	transformData.projMat = DirectX::XMMatrixTranspose(projMat);
	_constantBuffer->Update(&transformData);

	_mesh->Bind(rsState, samplerState);
	Graphic::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(0, 1, _constantBuffer->GetComPtr().GetAddressOf());

	if (lightConstantBuffer != nullptr)
	{
		Graphic::GetInstance()->GetDeviceContext()->PSSetConstantBuffers(1, 1, lightConstantBuffer->GetComPtr().GetAddressOf());
	}

	if (shadowConstantBuffer != nullptr)
	{
		Graphic::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(2, 1, shadowConstantBuffer->GetComPtr().GetAddressOf());
		Graphic::GetInstance()->GetDeviceContext()->PSSetConstantBuffers(2, 1, shadowConstantBuffer->GetComPtr().GetAddressOf());
	}

	if (shadowShaderResourceView != nullptr)
	{
		Graphic::GetInstance()->GetDeviceContext()->PSSetShaderResources(1, 1, &shadowShaderResourceView);
	}

	if (shadowSamplerState != nullptr)
	{
		Graphic::GetInstance()->GetDeviceContext()->PSSetSamplers(1, 1, &shadowSamplerState);
	}

	_mesh->Draw();

	if (_colliderVisible && _collider != nullptr)
	{
		_collider->Render(GetWorldMatrix(), viewMat, projMat);
	}
}

void EnemyObject::AttackToward(const DirectX::XMFLOAT3& targetPosition)
{
	if (_hitStunTime > 0.0f)
	{
		return;
	}

	DirectX::XMFLOAT3 direction(
		targetPosition.x - _position.x,
		0.0f,
		targetPosition.z - _position.z);
	SetTiltDirection(direction, 1.25f);
}

bool EnemyObject::TryAttackToward(const DirectX::XMFLOAT3& targetPosition)
{
	if (_hitStunTime > 0.0f || _attackCooldown > 0.0f)
	{
		return false;
	}

	AttackToward(targetPosition);
	_attackCooldown = _attackInterval;
	return true;
}

void EnemyObject::HitFrom(const DirectX::XMFLOAT3& attackerPosition)
{
	DirectX::XMFLOAT3 direction(
		_position.x - attackerPosition.x,
		0.0f,
		_position.z - attackerPosition.z);
	SetTiltDirection(direction, 0.95f);
	_hitStunTime = _hitStunDuration;
}

void EnemyObject::MoveRandom(float deltaTime)
{
	if (_hitStunTime > 0.0f)
	{
		return;
	}

	_wanderTime -= deltaTime;
	if (_wanderTime <= 0.0f)
	{
		const float random01 = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
		const float random02 = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
		const float angle = random01 * DirectX::XM_2PI;
		_wanderDirection = DirectX::XMFLOAT3(std::cos(angle), 0.0f, std::sin(angle));
		_wanderTime = 0.8f + random02 * 1.2f;
	}

	MoveInDirection(_wanderDirection, _moveSpeed, deltaTime);
}

void EnemyObject::MoveToward(const DirectX::XMFLOAT3& targetPosition, float deltaTime)
{
	if (_hitStunTime > 0.0f)
	{
		return;
	}

	DirectX::XMFLOAT3 direction(
		targetPosition.x - _position.x,
		0.0f,
		targetPosition.z - _position.z);
	MoveInDirection(direction, _chaseSpeed, deltaTime);
}

DirectX::XMMATRIX EnemyObject::GetWorldMatrix() const
{
	DirectX::XMVECTOR tiltDirection = DirectX::XMLoadFloat3(&_tiltDirection);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR axis = DirectX::XMVector3Cross(up, tiltDirection);

	if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(axis)) <= 0.0001f)
	{
		axis = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	}

	axis = DirectX::XMVector3Normalize(axis);

	return DirectX::XMMatrixRotationAxis(axis, _tiltAmount) *
		DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z);
}

void EnemyObject::SetPosition(const DirectX::XMFLOAT3& position)
{
	_position = position;
	ClampToMapBounds();
}

void EnemyObject::SetTiltDirection(const DirectX::XMFLOAT3& direction, float amount)
{
	DirectX::XMVECTOR directionVector = DirectX::XMLoadFloat3(&direction);
	if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(directionVector)) <= 0.0001f)
	{
		return;
	}

	directionVector = DirectX::XMVector3Normalize(directionVector);
	DirectX::XMStoreFloat3(&_tiltDirection, directionVector);
	_targetTiltAmount = amount;
}

void EnemyObject::MoveInDirection(const DirectX::XMFLOAT3& direction, float speed, float deltaTime)
{
	DirectX::XMVECTOR directionVector = DirectX::XMLoadFloat3(&direction);
	if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(directionVector)) <= 0.0001f)
	{
		return;
	}

	directionVector = DirectX::XMVector3Normalize(directionVector);
	DirectX::XMFLOAT3 normalizedDirection;
	DirectX::XMStoreFloat3(&normalizedDirection, directionVector);

	_position.x += normalizedDirection.x * speed * deltaTime;
	_position.z += normalizedDirection.z * speed * deltaTime;
	ClampToMapBounds();
}

void EnemyObject::ClampToMapBounds()
{
	constexpr float minBound = -5.1f;
	constexpr float maxBound = 5.1f;
	const float prevX = _position.x;
	const float prevZ = _position.z;

	_position.x = (std::max)(minBound, (std::min)(maxBound, _position.x));
	_position.z = (std::max)(minBound, (std::min)(maxBound, _position.z));

	if (prevX != _position.x)
	{
		_wanderDirection.x *= -1.0f;
	}

	if (prevZ != _position.z)
	{
		_wanderDirection.z *= -1.0f;
	}
}
