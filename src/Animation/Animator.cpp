#include "..\pch.h"

Animator::Animator()
{
	SetIdentityBoneTransforms();
}

bool Animator::AddAnimation(const std::string& name, const std::shared_ptr<Animation>& animation)
{
	if (name.empty() || animation == nullptr || !animation->IsValid())
	{
		return false;
	}

	_animations[name] = animation;

	if (_currentName.empty())
	{
		_currentName = name;
		_currentTime = 0.0;
		SampleCurrent();
	}

	return true;
}

bool Animator::Play(const std::string& name, float blendDuration, bool restart)
{
	auto iter = _animations.find(name);
	if (iter == _animations.end())
	{
		return false;
	}

	if (_currentName.empty())
	{
		_currentName = name;
		_currentTime = 0.0;
		_isBlending = false;
		SampleCurrent();
		return true;
	}

	if (_currentName == name && !_isBlending)
	{
		if (restart)
		{
			_currentTime = 0.0;
			SampleCurrent();
		}
		return true;
	}

	if (_isBlending && _nextName == name)
	{
		return true;
	}

	_nextName = name;
	_nextTime = 0.0;
	_blendDuration = (std::max)(0.0f, blendDuration);
	_blendElapsed = 0.0f;
	_isBlending = _blendDuration > 0.0f;

	if (!_isBlending)
	{
		_currentName = _nextName;
		_currentTime = _nextTime;
		_nextName.clear();
		SampleCurrent();
	}

	return true;
}

void Animator::Update(float deltaTime)
{
	if (_currentName.empty())
	{
		return;
	}

	std::shared_ptr<Animation> currentAnimation = _animations[_currentName];
	_currentTime = currentAnimation->AdvanceTime(_currentTime, deltaTime);

	if (!_isBlending || _nextName.empty())
	{
		SampleCurrent();
		return;
	}

	std::shared_ptr<Animation> nextAnimation = _animations[_nextName];
	_nextTime = nextAnimation->AdvanceTime(_nextTime, deltaTime);
	_blendElapsed += deltaTime;

	BoneTransformData currentPose;
	BoneTransformData nextPose;
	currentAnimation->Sample(_currentTime, currentPose);
	nextAnimation->Sample(_nextTime, nextPose);

	float blendRatio = _blendDuration <= 0.0f ? 1.0f : (std::min)(_blendElapsed / _blendDuration, 1.0f);
	BlendBoneTransformData(currentPose, nextPose, blendRatio, _boneTransformData);

	if (blendRatio >= 1.0f)
	{
		_currentName = _nextName;
		_currentTime = _nextTime;
		_nextName.clear();
		_nextTime = 0.0;
		_isBlending = false;
		SampleCurrent();
	}
}

bool Animator::HasAnimation(const std::string& name) const
{
	return _animations.find(name) != _animations.end();
}

void Animator::SampleCurrent()
{
	auto iter = _animations.find(_currentName);
	if (iter == _animations.end())
	{
		SetIdentityBoneTransforms();
		return;
	}

	iter->second->Sample(_currentTime, _boneTransformData);
}

void Animator::BlendBoneTransformData(
	const BoneTransformData& from,
	const BoneTransformData& to,
	float blendRatio,
	BoneTransformData& outBoneTransformData) const
{
	for (UINT idx = 0; idx < 128; ++idx)
	{
		DirectX::XMMATRIX fromMatrix = DirectX::XMMatrixTranspose(from.boneMats[idx]);
		DirectX::XMMATRIX toMatrix = DirectX::XMMatrixTranspose(to.boneMats[idx]);
		outBoneTransformData.boneMats[idx] = DirectX::XMMatrixTranspose(BlendMatrix(fromMatrix, toMatrix, blendRatio));
	}
}

DirectX::XMMATRIX Animator::BlendMatrix(
	const DirectX::XMMATRIX& from,
	const DirectX::XMMATRIX& to,
	float blendRatio) const
{
	DirectX::XMVECTOR fromScale;
	DirectX::XMVECTOR fromRotation;
	DirectX::XMVECTOR fromTranslation;
	DirectX::XMVECTOR toScale;
	DirectX::XMVECTOR toRotation;
	DirectX::XMVECTOR toTranslation;

	if (!DirectX::XMMatrixDecompose(&fromScale, &fromRotation, &fromTranslation, from) ||
		!DirectX::XMMatrixDecompose(&toScale, &toRotation, &toTranslation, to))
	{
		DirectX::XMMATRIX result;
		for (UINT row = 0; row < 4; ++row)
		{
			result.r[row] = DirectX::XMVectorLerp(from.r[row], to.r[row], blendRatio);
		}
		return result;
	}

	DirectX::XMVECTOR scale = DirectX::XMVectorLerp(fromScale, toScale, blendRatio);
	DirectX::XMVECTOR rotation = DirectX::XMQuaternionSlerp(fromRotation, toRotation, blendRatio);
	DirectX::XMVECTOR translation = DirectX::XMVectorLerp(fromTranslation, toTranslation, blendRatio);

	return DirectX::XMMatrixAffineTransformation(scale, DirectX::g_XMZero, rotation, translation);
}

void Animator::SetIdentityBoneTransforms()
{
	for (DirectX::XMMATRIX& boneMat : _boneTransformData.boneMats)
	{
		boneMat = DirectX::XMMatrixIdentity();
	}
}
