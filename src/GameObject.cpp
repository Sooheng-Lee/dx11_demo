#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	_constData.worldMat = DirectX::XMMatrixIdentity();
	_constData.viewMat = DirectX::XMMatrixIdentity();
	_constData.projMat = DirectX::XMMatrixIdentity();
	_lightConstData.directionalLightDirection = DirectX::XMFLOAT4(0.3f, -1.0f, 0.3f, 0.0f);
	_lightConstData.directionalLightColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_lightConstData.ambientColor = DirectX::XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	_lightConstData.useLight = 1;
	SetIdentityBoneTransforms();
}

bool GameObject::Init(
	const std::string& modelPath,
	const std::wstring& texturePath,
	const std::wstring& normalTexturePath,
	const std::wstring& roughnessTexturePath,
	const std::wstring& metallicTexturePath)
{
	_skeletalMesh = std::make_shared<SkeletalMesh>();
	_skeletalMesh->Init(modelPath, texturePath, normalTexturePath, roughnessTexturePath, metallicTexturePath);
	_lightConstData.useNormalTexture = normalTexturePath.empty() ? 0 : 1;
	_lightConstData.useRoughnessTexture = roughnessTexturePath.empty() ? 0 : 1;
	_lightConstData.useMetallicTexture = metallicTexturePath.empty() ? 0 : 1;
	CreateConstantBuffer();

	return true;
}

bool GameObject::AddAnimation(const std::string& name, const std::string& animationPath)
{
	if (name.empty() || animationPath.empty())
	{
		return false;
	}

	if (_skeletalMesh == nullptr)
	{
		return false;
	}

	if (_animator == nullptr)
	{
		_animator = std::make_shared<Animator>();
	}

	std::shared_ptr<Animation> animation = std::make_shared<Animation>();
	if (!animation->Load(animationPath, _skeletalMesh->GetBoneInfoMap(), _skeletalMesh->GetBoneCount()))
	{
		return false;
	}

	const bool result = _animator->AddAnimation(name, animation);
	_boneConstData = _animator->GetBoneTransformData();
	return result;
}

bool GameObject::PlayAnimation(const std::string& name, float blendDuration, bool restart)
{
	if (_animator == nullptr)
	{
		return false;
	}

	return _animator->Play(name, blendDuration, restart);
}

double GameObject::GetAnimationDurationSeconds(const std::string& name) const
{
	if (_animator == nullptr)
	{
		return 0.0;
	}

	return _animator->GetAnimationDurationSeconds(name);
}

void GameObject::SetDirectionalLight(const DirectionalLight& light)
{
	const DirectX::XMFLOAT3& direction = light.GetDirection();
	const DirectX::XMFLOAT3& color = light.GetColor();
	const float intensity = light.GetIntensity();

	_lightConstData.directionalLightDirection = DirectX::XMFLOAT4(direction.x, direction.y, direction.z, 0.0f);
	_lightConstData.directionalLightColor = DirectX::XMFLOAT4(
		color.x * intensity,
		color.y * intensity,
		color.z * intensity,
		1.0f);

	if (_lightConstantBuffer != nullptr)
	{
		_lightConstantBuffer->Update(&_lightConstData);
	}
}

void GameObject::SetLightEnabled(bool enabled)
{
	_lightConstData.useLight = enabled ? 1 : 0;

	if (_lightConstantBuffer != nullptr)
	{
		_lightConstantBuffer->Update(&_lightConstData);
	}
}

void GameObject::SetNormalTextureEnabled(bool enabled)
{
	_lightConstData.useNormalTexture = enabled ? 1 : 0;

	if (_skeletalMesh != nullptr)
	{
		_skeletalMesh->SetNormalTextureEnabled(enabled);
	}

	if (_lightConstantBuffer != nullptr)
	{
		_lightConstantBuffer->Update(&_lightConstData);
	}
}

void GameObject::SetRoughnessTextureEnabled(bool enabled)
{
	_lightConstData.useRoughnessTexture = enabled ? 1 : 0;

	if (_skeletalMesh != nullptr)
	{
		_skeletalMesh->SetRoughnessTextureEnabled(enabled);
	}

	if (_lightConstantBuffer != nullptr)
	{
		_lightConstantBuffer->Update(&_lightConstData);
	}
}

void GameObject::SetMetallicTextureEnabled(bool enabled)
{
	_lightConstData.useMetallicTexture = enabled ? 1 : 0;

	if (_skeletalMesh != nullptr)
	{
		_skeletalMesh->SetMetallicTextureEnabled(enabled);
	}

	if (_lightConstantBuffer != nullptr)
	{
		_lightConstantBuffer->Update(&_lightConstData);
	}
}

void GameObject::ToggleLightEnabled()
{
	SetLightEnabled(!IsLightEnabled());
}

void GameObject::SetCollider(const std::shared_ptr<Collider>& collider)
{
	_collider = collider;
}

void GameObject::ToggleColliderVisible()
{
	SetColliderVisible(!_colliderVisible);
}

DirectX::XMFLOAT3 GameObject::GetForwardVector() const
{
	return DirectX::XMFLOAT3(-std::sin(_rotationY), 0.0f, -std::cos(_rotationY));
}

DirectX::XMMATRIX GameObject::GetWorldMatrix() const
{
	return DirectX::XMMatrixRotationY(_rotationY) *
		DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z);
}

void GameObject::Move(const DirectX::XMFLOAT3& direction, float deltaTime)
{
	DirectX::XMVECTOR moveDirection = DirectX::XMLoadFloat3(&direction);
	if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(moveDirection)) <= 0.0001f)
	{
		return;
	}

	moveDirection = DirectX::XMVector3Normalize(moveDirection);
	DirectX::XMFLOAT3 normalizedDirection;
	DirectX::XMStoreFloat3(&normalizedDirection, moveDirection);

	_position.x += normalizedDirection.x * _moveSpeed * deltaTime;
	_position.y += normalizedDirection.y * _moveSpeed * deltaTime;
	_position.z += normalizedDirection.z * _moveSpeed * deltaTime;
	_targetRotationY = NormalizeAngle(std::atan2(normalizedDirection.x, normalizedDirection.z) + DirectX::XM_PI);
}

void GameObject::Update(float deltaTime, const DirectX::XMMATRIX& viewMat, const DirectX::XMMATRIX& projMat)
{
	if (CanUpdateRotation())
	{
		UpdateRotation(deltaTime);
	}

	_worldMat = GetWorldMatrix();
	_viewMat = viewMat;
	_projMat = projMat;

	_constData.worldMat = DirectX::XMMatrixTranspose(_worldMat);
	_constData.viewMat = DirectX::XMMatrixTranspose(viewMat);
	_constData.projMat = DirectX::XMMatrixTranspose(projMat);
	_constantBuffer->Update(&_constData);

	if (_animator != nullptr)
	{
		_animator->Update(deltaTime);
		_boneConstData = _animator->GetBoneTransformData();
		_boneConstantBuffer->Update(&_boneConstData);
	}
}

void GameObject::Render(
	ID3D11RasterizerState* rsState,
	ID3D11BlendState* blendState,
	ID3D11SamplerState* samplerState)
{
	if (_skeletalMesh == nullptr)
	{
		return;
	}

	_constData.worldMat = DirectX::XMMatrixTranspose(_worldMat);
	_constData.viewMat = DirectX::XMMatrixTranspose(_viewMat);
	_constData.projMat = DirectX::XMMatrixTranspose(_projMat);
	_constantBuffer->Update(&_constData);

	_skeletalMesh->Bind(rsState, samplerState);
	Graphic::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(0, 1, _constantBuffer->GetComPtr().GetAddressOf());
	Graphic::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(1, 1, _boneConstantBuffer->GetComPtr().GetAddressOf());
	Graphic::GetInstance()->GetDeviceContext()->PSSetConstantBuffers(0, 1, _lightConstantBuffer->GetComPtr().GetAddressOf());

	FLOAT blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	Graphic::GetInstance()->GetDeviceContext()->OMSetBlendState(blendState, blendFactor, 0xFFFFFFFF);
	_skeletalMesh->Draw();

	if (_colliderVisible && _collider != nullptr)
	{
		_collider->Render(_worldMat, _viewMat, _projMat);
	}
}

void GameObject::RenderShadow(
	ID3D11RasterizerState* rsState,
	VertexShader* shadowVertexShader,
	const DirectX::XMMATRIX& lightViewMat,
	const DirectX::XMMATRIX& lightProjMat)
{
	if (_skeletalMesh == nullptr || shadowVertexShader == nullptr)
	{
		return;
	}

	TransformData shadowTransformData;
	shadowTransformData.worldMat = DirectX::XMMatrixTranspose(GetWorldMatrix());
	shadowTransformData.viewMat = DirectX::XMMatrixTranspose(lightViewMat);
	shadowTransformData.projMat = DirectX::XMMatrixTranspose(lightProjMat);
	_constantBuffer->Update(&shadowTransformData);

	_skeletalMesh->BindShadow(rsState, shadowVertexShader);
	Graphic::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(0, 1, _constantBuffer->GetComPtr().GetAddressOf());
	Graphic::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(1, 1, _boneConstantBuffer->GetComPtr().GetAddressOf());
	_skeletalMesh->Draw();
}

void GameObject::CreateConstantBuffer()
{
	_constantBuffer = std::make_shared<ConstantBuffer<TransformData>>();
	_constantBuffer->Create();

	_boneConstantBuffer = std::make_shared<ConstantBuffer<BoneTransformData>>();
	_boneConstantBuffer->Create();
	_boneConstantBuffer->Update(&_boneConstData);

	_lightConstantBuffer = std::make_shared<ConstantBuffer<LightData>>();
	_lightConstantBuffer->Create();
	_lightConstantBuffer->Update(&_lightConstData);
}

void GameObject::SetIdentityBoneTransforms()
{
	for (DirectX::XMMATRIX& boneMat : _boneConstData.boneMats)
	{
		boneMat = DirectX::XMMatrixIdentity();
	}
}

void GameObject::UpdateRotation(float deltaTime)
{
	const float angleDelta = NormalizeAngle(_targetRotationY - _rotationY);
	const float lerpRatio = (std::min)(deltaTime * _rotationLerpSpeed, 1.0f);

	if (std::fabs(angleDelta) <= 0.001f)
	{
		_rotationY = _targetRotationY;
		return;
	}

	_rotationY = NormalizeAngle(_rotationY + angleDelta * lerpRatio);
}

float GameObject::NormalizeAngle(float angle) const
{
	while (angle > DirectX::XM_PI)
	{
		angle -= DirectX::XM_2PI;
	}

	while (angle < -DirectX::XM_PI)
	{
		angle += DirectX::XM_2PI;
	}

	return angle;
}
