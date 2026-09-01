#pragma once
#include "Client.h"
class Engine : public Client
{
public:
	Engine();
	~Engine();
	bool Init();

protected:
	virtual void Update() override;
	virtual void Render() override;

private:
	void CreateRSState();
	void CreateBlendState();
	void CreateSamplerState();
	void CreateShadowResources();
	void UpdateShadowMatrix();
	void RenderShadowMap();
	void CreateMapObjects();
	void AddMapObject(const MeshData<VertexTexData>& meshData, const DirectX::XMMATRIX& worldMat);
	void AddMapObject(const MeshData<VertexTexData>& meshData, const DirectX::XMMATRIX& worldMat, const DirectX::XMFLOAT3& colliderSize);
	void RenderMapObjects();
	void CreateEnemyObjects();
	void UpdateEnemies(float deltaTime);
	void ResolveEnemyOverlaps();
	void RenderEnemyObjects();
	void CreateAttackCollider();
	void UpdateAttackCollider(float deltaTime);
	void ActivateAttackCollider();
	void ApplyAttackColliderToEnemies();
	DirectX::XMMATRIX GetAttackColliderWorldMatrix() const;
	void RenderAttackCollider();
	bool IsCharacterColliding() const;
	bool IsCollidingWithObject(const Collider& collider, const DirectX::XMMATRIX& worldMat) const;
	void ResolveCharacterCollision(const DirectX::XMFLOAT3& prevPosition, const DirectX::XMFLOAT3& moveDirection);
	void KnockBackCharacterFrom(const DirectX::XMFLOAT3& sourcePosition);
	void UpdateCamera();
	void UpdateCharacterAnimation(bool isMoving, float deltaTime);

private:
	struct MapObject
	{
		std::shared_ptr<StaticMesh> mesh;
		std::shared_ptr<ConstantBuffer<TransformData>> constantBuffer;
		std::shared_ptr<Collider> collider;
		DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();
	};

	ComPtr<ID3D11RasterizerState> _rsState;
	ComPtr<ID3D11BlendState> _blendState;
	ComPtr<ID3D11SamplerState> _samplerState;
	ComPtr<ID3D11DepthStencilView> _shadowDepthStencilView;
	ComPtr<ID3D11ShaderResourceView> _shadowShaderResourceView;
	ComPtr<ID3D11SamplerState> _shadowSamplerState;
	std::shared_ptr<VertexShader> _shadowVertexShader;
	std::shared_ptr<ConstantBuffer<ShadowData>> _shadowConstantBuffer;
	ShadowData _shadowData;
	DirectX::XMMATRIX _lightViewMat = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX _lightProjMat = DirectX::XMMatrixIdentity();
	UINT _shadowMapSize = 2048;
	std::vector<MapObject> _mapObjects;
	std::vector<std::shared_ptr<EnemyObject>> _enemies;
	std::shared_ptr<ConstantBuffer<LightData>> _mapLightConstantBuffer;
	LightData _mapLightData;

	std::shared_ptr<Camera> _camera;
	std::shared_ptr<GameObject> _character;
	std::shared_ptr<BoxCollider> _attackCollider;
	enum class eAttackState
	{
		None,
		Punch1,
		Punch2
	};
	eAttackState _attackState = eAttackState::None;
	float _attackElapsed = 0.0f;
	float _punchExtendTime = 20.0f / 30.0f;
	float _attackColliderActiveTime = 0.0f;
	float _attackColliderDuration = 0.12f;
	bool _attackHitTriggered = false;
};

