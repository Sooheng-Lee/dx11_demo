#pragma once

class EnemyObject;

class PlayerCharacter : public GameObject
{
public:
	PlayerCharacter() = default;
	~PlayerCharacter() override = default;

	void CreateAttackCollider();
	void UpdateCombat(bool attackPressed, bool isMoving, float deltaTime, const std::vector<std::shared_ptr<EnemyObject>>& enemies);
	void RenderAttackCollider(const DirectX::XMMATRIX& viewMat, const DirectX::XMMATRIX& projMat);
	void HitFrom(const DirectX::XMFLOAT3& attackerPosition);
	void MoveForwardDuringAttack(float deltaTime);
	bool IsAttacking() const { return _attackState != eAttackState::None; }
	bool IsKnockingBack() const { return _isKnockingBack; }

private:
	enum class eAttackState
	{
		None,
		Punch1,
		Punch2
	};

	bool CanUpdateRotation() const override { return !IsAttacking(); }
	void UpdateAttackCollider(float deltaTime, const std::vector<std::shared_ptr<EnemyObject>>& enemies);
	void ActivateAttackCollider(const std::vector<std::shared_ptr<EnemyObject>>& enemies);
	void ApplyAttackColliderToEnemies(const std::vector<std::shared_ptr<EnemyObject>>& enemies);
	DirectX::XMMATRIX GetAttackColliderWorldMatrix() const;
	void UpdateKnockBack(float deltaTime);
	void ResetAttack();

private:
	std::shared_ptr<BoxCollider> _attackCollider;
	eAttackState _attackState = eAttackState::None;
	float _attackElapsed = 0.0f;
	float _punchExtendTime = 20.0f / 30.0f;
	float _attackColliderActiveTime = 0.0f;
	float _attackColliderDuration = 0.12f;
	float _attackMoveSpeed = 1.15f;
	bool _attackHitTriggered = false;
	bool _isKnockingBack = false;
	float _knockBackElapsed = 0.0f;
	float _knockBackDuration = 0.18f;
	float _knockBackDistance = 0.55f;
	DirectX::XMFLOAT3 _knockBackStartPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 _knockBackTargetPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	std::vector<EnemyObject*> _hitEnemiesThisAttack;
};
