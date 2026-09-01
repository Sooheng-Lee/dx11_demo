#pragma once

class EnemyObject : public GameObject
{
public:
	EnemyObject() = default;
	~EnemyObject() override = default;

	bool Init(const DirectX::XMFLOAT3& position);
	void Update(float deltaTime);
	void Render(
		const DirectX::XMMATRIX& viewMat,
		const DirectX::XMMATRIX& projMat,
		ID3D11RasterizerState* rsState,
		ID3D11SamplerState* samplerState,
		const std::shared_ptr<ConstantBuffer<LightData>>& lightConstantBuffer,
		const std::shared_ptr<ConstantBuffer<ShadowData>>& shadowConstantBuffer,
		ID3D11ShaderResourceView* shadowShaderResourceView,
		ID3D11SamplerState* shadowSamplerState);

	void AttackToward(const DirectX::XMFLOAT3& targetPosition);
	bool TryAttackToward(const DirectX::XMFLOAT3& targetPosition);
	void HitFrom(const DirectX::XMFLOAT3& attackerPosition);
	void MoveRandom(float deltaTime);
	void MoveToward(const DirectX::XMFLOAT3& targetPosition, float deltaTime);
	void SetColliderVisible(bool visible) { _colliderVisible = visible; }
	DirectX::XMMATRIX GetWorldMatrix() const;
	void SetPosition(const DirectX::XMFLOAT3& position);

private:
	void SetTiltDirection(const DirectX::XMFLOAT3& direction, float amount);
	void MoveInDirection(const DirectX::XMFLOAT3& direction, float speed, float deltaTime);
	void ClampToMapBounds();

private:
	std::shared_ptr<StaticMesh> _mesh;
	DirectX::XMFLOAT3 _tiltDirection = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT3 _wanderDirection = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 _baseColor = DirectX::XMFLOAT4(0.9f, 0.05f, 0.04f, 1.0f);
	DirectX::XMFLOAT4 _hitColor = DirectX::XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f);
	float _tiltAmount = 0.0f;
	float _targetTiltAmount = 0.0f;
	float _tiltReturnSpeed = 5.0f;
	float _hitStunTime = 0.0f;
	float _hitStunDuration = 0.45f;
	float _wanderTime = 0.0f;
	float _moveSpeed = 0.9f;
	float _chaseSpeed = 1.45f;
	float _attackCooldown = 0.0f;
	float _attackInterval = 1.15f;
};
