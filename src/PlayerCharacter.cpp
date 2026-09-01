#include "pch.h"

void PlayerCharacter::CreateAttackCollider()
{
	_attackCollider = std::make_shared<BoxCollider>(DirectX::XMFLOAT3(0.9f, 0.9f, 0.75f));
	_attackCollider->SetCenter(DirectX::XMFLOAT3(0.0f, 0.9f, 0.0f));
	_attackCollider->Init();
}

void PlayerCharacter::UpdateCombat(
	bool attackPressed,
	bool isMoving,
	float deltaTime,
	const std::vector<std::shared_ptr<EnemyObject>>& enemies)
{
	UpdateKnockBack(deltaTime);

	if (_attackState != eAttackState::None)
	{
		_attackElapsed += deltaTime;
	}

	if (attackPressed)
	{
		if (_attackState == eAttackState::Punch1 && _attackElapsed >= _punchExtendTime)
		{
			PlayAnimation("Punch2", 0.1f, true);
			_attackState = eAttackState::Punch2;
			_attackElapsed = 0.0f;
			_attackHitTriggered = false;
			_hitEnemiesThisAttack.clear();
		}
		else if (_attackState == eAttackState::Punch2 && _attackElapsed >= _punchExtendTime)
		{
			PlayAnimation("Punch1", 0.1f, true);
			_attackState = eAttackState::Punch1;
			_attackElapsed = 0.0f;
			_attackHitTriggered = false;
			_hitEnemiesThisAttack.clear();
		}
		else if (_attackState == eAttackState::None)
		{
			PlayAnimation("Punch1", 0.1f, true);
			_attackState = eAttackState::Punch1;
			_attackElapsed = 0.0f;
			_attackHitTriggered = false;
			_hitEnemiesThisAttack.clear();
		}
	}

	if (_attackState != eAttackState::None && !_attackHitTriggered && _attackElapsed >= _punchExtendTime)
	{
		ActivateAttackCollider(enemies);
		_attackHitTriggered = true;
	}

	if (_attackState == eAttackState::Punch1)
	{
		const double duration = GetAnimationDurationSeconds("Punch1");
		if (duration > 0.0 && _attackElapsed >= static_cast<float>(duration))
		{
			ResetAttack();
		}
	}
	else if (_attackState == eAttackState::Punch2)
	{
		const double duration = GetAnimationDurationSeconds("Punch2");
		if (duration > 0.0 && _attackElapsed >= static_cast<float>(duration))
		{
			ResetAttack();
		}
	}

	UpdateAttackCollider(deltaTime, enemies);

	if (_attackState != eAttackState::None)
	{
		return;
	}

	PlayAnimation(isMoving ? "Walk" : "Idle", 0.2f);
}

void PlayerCharacter::RenderAttackCollider(const DirectX::XMMATRIX& viewMat, const DirectX::XMMATRIX& projMat)
{
	if (_attackCollider == nullptr || !IsColliderVisible() || _attackColliderActiveTime <= 0.0f)
	{
		return;
	}

	_attackCollider->Render(GetAttackColliderWorldMatrix(), viewMat, projMat);
}

void PlayerCharacter::UpdateAttackCollider(
	float deltaTime,
	const std::vector<std::shared_ptr<EnemyObject>>& enemies)
{
	if (_attackColliderActiveTime > 0.0f)
	{
		_attackColliderActiveTime = (std::max)(0.0f, _attackColliderActiveTime - deltaTime);
		ApplyAttackColliderToEnemies(enemies);
	}
}

void PlayerCharacter::ActivateAttackCollider(const std::vector<std::shared_ptr<EnemyObject>>& enemies)
{
	_attackColliderActiveTime = _attackColliderDuration;
	ApplyAttackColliderToEnemies(enemies);
}

void PlayerCharacter::ApplyAttackColliderToEnemies(const std::vector<std::shared_ptr<EnemyObject>>& enemies)
{
	if (_attackCollider == nullptr || _attackColliderActiveTime <= 0.0f)
	{
		return;
	}

	const DirectX::XMFLOAT3 characterPosition = GetPosition();
	const ColliderAABB attackAABB = _attackCollider->GetWorldAABB(GetAttackColliderWorldMatrix());

	for (const std::shared_ptr<EnemyObject>& enemy : enemies)
	{
		if (enemy == nullptr || enemy->IsDead() || enemy->GetCollider() == nullptr)
		{
			continue;
		}

		EnemyObject* enemyPtr = enemy.get();
		if (std::find(_hitEnemiesThisAttack.begin(), _hitEnemiesThisAttack.end(), enemyPtr) != _hitEnemiesThisAttack.end())
		{
			continue;
		}

		const ColliderAABB enemyAABB = enemy->GetCollider()->GetWorldAABB(enemy->GetWorldMatrix());
		if (Collider::Intersects(attackAABB, enemyAABB))
		{
			enemy->HitFrom(characterPosition);
			_hitEnemiesThisAttack.push_back(enemyPtr);
		}
	}
}

DirectX::XMMATRIX PlayerCharacter::GetAttackColliderWorldMatrix() const
{
	const DirectX::XMFLOAT3 characterPosition = GetPosition();
	const DirectX::XMFLOAT3 forward = GetForwardVector();
	const DirectX::XMFLOAT3 attackPosition(
		characterPosition.x + forward.x * 0.85f,
		characterPosition.y,
		characterPosition.z + forward.z * 0.85f);
	const float yaw = std::atan2(forward.x, forward.z);

	return DirectX::XMMatrixRotationY(yaw) *
		DirectX::XMMatrixTranslation(attackPosition.x, attackPosition.y, attackPosition.z);
}

void PlayerCharacter::HitFrom(const DirectX::XMFLOAT3& attackerPosition)
{
	const DirectX::XMFLOAT3 characterPosition = GetPosition();
	DirectX::XMFLOAT3 direction(
		characterPosition.x - attackerPosition.x,
		0.0f,
		characterPosition.z - attackerPosition.z);
	DirectX::XMVECTOR directionVector = DirectX::XMLoadFloat3(&direction);
	if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(directionVector)) <= 0.0001f)
	{
		directionVector = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	}

	directionVector = DirectX::XMVector3Normalize(directionVector);
	DirectX::XMStoreFloat3(&direction, directionVector);

	_knockBackStartPosition = characterPosition;
	_knockBackTargetPosition = characterPosition;
	_knockBackTargetPosition.x += direction.x * _knockBackDistance;
	_knockBackTargetPosition.z += direction.z * _knockBackDistance;
	_knockBackTargetPosition.x = (std::max)(-5.1f, (std::min)(5.1f, _knockBackTargetPosition.x));
	_knockBackTargetPosition.z = (std::max)(-5.1f, (std::min)(5.1f, _knockBackTargetPosition.z));
	_knockBackElapsed = 0.0f;
	_isKnockingBack = true;
}

void PlayerCharacter::MoveForwardDuringAttack(float deltaTime)
{
	const DirectX::XMFLOAT3 forward = GetForwardVector();
	_position.x += forward.x * _attackMoveSpeed * deltaTime;
	_position.z += forward.z * _attackMoveSpeed * deltaTime;
	_position.x = (std::max)(-5.1f, (std::min)(5.1f, _position.x));
	_position.z = (std::max)(-5.1f, (std::min)(5.1f, _position.z));
}

void PlayerCharacter::UpdateKnockBack(float deltaTime)
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

	if (t >= 1.0f)
	{
		_isKnockingBack = false;
	}
}

void PlayerCharacter::ResetAttack()
{
	_attackState = eAttackState::None;
	_attackElapsed = 0.0f;
	_attackHitTriggered = false;
	_hitEnemiesThisAttack.clear();
}
