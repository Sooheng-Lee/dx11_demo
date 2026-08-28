#pragma once

class DirectionalLight;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject() = default;

	bool Init(
		const std::string& modelPath,
		const std::wstring& texturePath);
	bool AddAnimation(const std::string& name, const std::string& animationPath);
	bool PlayAnimation(const std::string& name, float blendDuration = 0.2f, bool restart = false);
	void SetDirectionalLight(const DirectionalLight& light);
	void SetLightEnabled(bool enabled);
	bool IsLightEnabled() const { return _lightConstData.useLight != 0; }
	void ToggleLightEnabled();
	void Move(const DirectX::XMFLOAT3& direction, float deltaTime);
	virtual void Update(float deltaTime, const DirectX::XMMATRIX& viewMat, const DirectX::XMMATRIX& projMat);
	virtual void Render(
		ID3D11RasterizerState* rsState,
		ID3D11BlendState* blendState,
		ID3D11SamplerState* samplerState);

protected:
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
	float _rotationY = 0.0f;
	float _targetRotationY = 0.0f;
	float _rotationLerpSpeed = 8.0f;
	float _moveSpeed = 3.0f;
};
