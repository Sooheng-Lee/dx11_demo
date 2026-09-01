#pragma once

class DirectionalLight;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject() = default;

	bool Init(
		const std::string& modelPath,
		const std::wstring& texturePath,
		const std::wstring& normalTexturePath = L"",
		const std::wstring& roughnessTexturePath = L"",
		const std::wstring& metallicTexturePath = L"");
	bool AddAnimation(const std::string& name, const std::string& animationPath);
	bool PlayAnimation(const std::string& name, float blendDuration = 0.2f, bool restart = false);
	double GetAnimationDurationSeconds(const std::string& name) const;
	void SetDirectionalLight(const DirectionalLight& light);
	void SetLightEnabled(bool enabled);
	void SetNormalTextureEnabled(bool enabled);
	void SetRoughnessTextureEnabled(bool enabled);
	void SetMetallicTextureEnabled(bool enabled);
	bool IsLightEnabled() const { return _lightConstData.useLight != 0; }
	void ToggleLightEnabled();
	void SetCollider(const std::shared_ptr<Collider>& collider);
	std::shared_ptr<Collider> GetCollider() const { return _collider; }
	void SetColliderVisible(bool visible) { _colliderVisible = visible; }
	bool IsColliderVisible() const { return _colliderVisible; }
	void ToggleColliderVisible();
	void SetPosition(const DirectX::XMFLOAT3& position) { _position = position; }
	const DirectX::XMFLOAT3& GetPosition() const { return _position; }
	DirectX::XMFLOAT3 GetForwardVector() const;
	DirectX::XMMATRIX GetWorldMatrix() const;
	void Move(const DirectX::XMFLOAT3& direction, float deltaTime);
	virtual void Update(float deltaTime, const DirectX::XMMATRIX& viewMat, const DirectX::XMMATRIX& projMat);
	virtual void Render(
		ID3D11RasterizerState* rsState,
		ID3D11BlendState* blendState,
		ID3D11SamplerState* samplerState);
	void RenderShadow(
		ID3D11RasterizerState* rsState,
		VertexShader* shadowVertexShader,
		const DirectX::XMMATRIX& lightViewMat,
		const DirectX::XMMATRIX& lightProjMat);

protected:
	virtual bool CanUpdateRotation() const { return true; }
	void CreateConstantBuffer();
	void SetIdentityBoneTransforms();
	void UpdateRotation(float deltaTime);
	float NormalizeAngle(float angle) const;

protected:
	std::shared_ptr<SkeletalMesh> _skeletalMesh;
	std::shared_ptr<ConstantBuffer<TransformData>> _constantBuffer;
	std::shared_ptr<ConstantBuffer<BoneTransformData>> _boneConstantBuffer;
	std::shared_ptr<ConstantBuffer<LightData>> _lightConstantBuffer;
	std::shared_ptr<Animator> _animator;

	TransformData _constData;
	BoneTransformData _boneConstData;
	LightData _lightConstData;
	DirectX::XMFLOAT3 _position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX _worldMat = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX _viewMat = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX _projMat = DirectX::XMMatrixIdentity();
	float _rotationY = 0.0f;
	float _targetRotationY = 0.0f;
	float _rotationLerpSpeed = 8.0f;
	float _moveSpeed = 3.0f;
	std::shared_ptr<Collider> _collider;
	bool _colliderVisible = false;
};
