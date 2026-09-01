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
	if (_isDead)
	{
		return;
	}

	UpdateKnockBack(deltaTime);

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
	if (_isDead || _mesh == nullptr || _constantBuffer == nullptr)
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
	if (_isDead || _hitStunTime > 0.0f)
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
	if (_isDead || _hitStunTime > 0.0f || _attackCooldown > 0.0f)
	{
		return false;
	}

	AttackToward(targetPosition);
	_attackCooldown = _attackInterval;
	return true;
}

void EnemyObject::HitFrom(const DirectX::XMFLOAT3& attackerPosition)
{
	if (_isDead)
	{
		return;
	}

	++_hitCount;
	if (_hitCount >= _maxHitCount)
	{
		_isDead = true;
		return;
	}

	DirectX::XMFLOAT3 direction(
		_position.x - attackerPosition.x,
		0.0f,
		_position.z - attackerPosition.z);
	SetTiltDirection(direction, 0.95f);
	StartKnockBackFrom(attackerPosition);
	_hitStunTime = _hitStunDuration;
}

void EnemyObject::MoveRandom(float deltaTime)
{
	if (_isDead || _hitStunTime > 0.0f || _isKnockingBack)
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
	if (_isDead || _hitStunTime > 0.0f || _isKnockingBack)
	{
		return;
	}

	DirectX::XMFLOAT3 direction(
		targetPosition.x - _position.x,
		0.0f,
		targetPosition.z - _position.z);
	MoveInDirection(direction, _chaseSpeed, deltaTime);
}

void EnemyObject::UpdateEnemies(
	std::vector<std::shared_ptr<EnemyObject>>& enemies,
	const DirectX::XMFLOAT3& targetPosition,
	float deltaTime,
	const std::function<void(const DirectX::XMFLOAT3&)>& onAttackHit)
{
	for (const std::shared_ptr<EnemyObject>& enemy : enemies)
	{
		if (enemy != nullptr && !enemy->IsDead())
		{
			enemy->Update(deltaTime);
		}
	}

	enemies.erase(
		std::remove_if(
			enemies.begin(),
			enemies.end(),
			[](const std::shared_ptr<EnemyObject>& enemy)
			{
				return enemy == nullptr || enemy->IsDead();
			}),
		enemies.end());

	constexpr float enemyDetectRange = 4.0f;
	constexpr float enemyAttackRange = 1.35f;
	const DirectX::XMVECTOR targetVector = DirectX::XMLoadFloat3(&targetPosition);

	for (const std::shared_ptr<EnemyObject>& enemy : enemies)
	{
		if (enemy == nullptr || enemy->IsDead())
		{
			continue;
		}

		const DirectX::XMFLOAT3 enemyPosition = enemy->GetPosition();
		const DirectX::XMVECTOR enemyVector = DirectX::XMLoadFloat3(&enemyPosition);
		const float closestDistanceSq = DirectX::XMVectorGetX(
			DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(targetVector, enemyVector)));

		if (closestDistanceSq <= enemyDetectRange * enemyDetectRange)
		{
			if (closestDistanceSq > enemyAttackRange * enemyAttackRange)
			{
				enemy->MoveToward(targetPosition, deltaTime);
			}
			else if (enemy->TryAttackToward(targetPosition) && onAttackHit)
			{
				onAttackHit(enemyPosition);
			}
		}
		else
		{
			enemy->MoveRandom(deltaTime);
		}
	}

	ResolveOverlaps(enemies);
}

void EnemyObject::ResolveOverlaps(std::vector<std::shared_ptr<EnemyObject>>& enemies)
{
	constexpr float minDistance = 0.85f;
	constexpr float minDistanceSq = minDistance * minDistance;
	constexpr float pushEpsilon = 0.02f;

	for (size_t lhsIndex = 0; lhsIndex < enemies.size(); ++lhsIndex)
	{
		const std::shared_ptr<EnemyObject>& lhs = enemies[lhsIndex];
		if (lhs == nullptr || lhs->IsDead())
		{
			continue;
		}

		for (size_t rhsIndex = lhsIndex + 1; rhsIndex < enemies.size(); ++rhsIndex)
		{
			const std::shared_ptr<EnemyObject>& rhs = enemies[rhsIndex];
			if (rhs == nullptr || rhs->IsDead())
			{
				continue;
			}

			const DirectX::XMFLOAT3 lhsPosition = lhs->GetPosition();
			const DirectX::XMFLOAT3 rhsPosition = rhs->GetPosition();
			DirectX::XMFLOAT3 delta(
				rhsPosition.x - lhsPosition.x,
				0.0f,
				rhsPosition.z - lhsPosition.z);
			DirectX::XMVECTOR deltaVector = DirectX::XMLoadFloat3(&delta);
			float distanceSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(deltaVector));

			if (distanceSq >= minDistanceSq)
			{
				continue;
			}

			if (distanceSq <= 0.0001f)
			{
				delta = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
				deltaVector = DirectX::XMLoadFloat3(&delta);
				distanceSq = 1.0f;
			}

			const float distance = std::sqrt(distanceSq);
			const float pushDistance = (minDistance - distance) * 0.5f + pushEpsilon;
			deltaVector = DirectX::XMVector3Normalize(deltaVector);
			DirectX::XMStoreFloat3(&delta, deltaVector);

			DirectX::XMFLOAT3 newLhsPosition = lhsPosition;
			newLhsPosition.x -= delta.x * pushDistance;
			newLhsPosition.z -= delta.z * pushDistance;

			DirectX::XMFLOAT3 newRhsPosition = rhsPosition;
			newRhsPosition.x += delta.x * pushDistance;
			newRhsPosition.z += delta.z * pushDistance;

			lhs->SetPosition(newLhsPosition);
			rhs->SetPosition(newRhsPosition);
		}
	}
}

void EnemyObject::StartKnockBackFrom(const DirectX::XMFLOAT3& sourcePosition)
{
	DirectX::XMFLOAT3 direction(
		_position.x - sourcePosition.x,
		0.0f,
		_position.z - sourcePosition.z);
	DirectX::XMVECTOR directionVector = DirectX::XMLoadFloat3(&direction);
	if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(directionVector)) <= 0.0001f)
	{
		directionVector = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	}

	directionVector = DirectX::XMVector3Normalize(directionVector);
	DirectX::XMStoreFloat3(&direction, directionVector);

	_knockBackStartPosition = _position;
	_knockBackTargetPosition = _position;
	_knockBackTargetPosition.x += direction.x * _knockBackDistance;
	_knockBackTargetPosition.z += direction.z * _knockBackDistance;
	_knockBackElapsed = 0.0f;
	_isKnockingBack = true;

	_position = _knockBackTargetPosition;
	ClampToMapBounds();
	_knockBackTargetPosition = _position;
	_position = _knockBackStartPosition;
}

void EnemyObject::UpdateKnockBack(float deltaTime)
{
	if (!_isKnockingBack)
	{
		return;
	}

	_knockBackElapsed += deltaTime;
	const float t = _knockBackDuration > 0.0f
		? (std::min)(_knockBackElapsed / _knockBackDuration, 1.0f)
		: 1.0f;

	_position.x = _knockBackStartPosition.x + (_knockBackTargetPosition.x - _knockBackStartPosition.x) * t;
	_position.y = _knockBackStartPosition.y + (_knockBackTargetPosition.y - _knockBackStartPosition.y) * t;
	_position.z = _knockBackStartPosition.z + (_knockBackTargetPosition.z - _knockBackStartPosition.z) * t;
	ClampToMapBounds();

	if (t >= 1.0f)
	{
		_isKnockingBack = false;
	}
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
