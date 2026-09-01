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

double Animator::GetCurrentAnimationDurationSeconds() const
{
	return GetAnimationDurationSeconds(_currentName);
}

double Animator::GetAnimationDurationSeconds(const std::string& name) const
{
	auto iter = _animations.find(name);
	if (iter == _animations.end() || iter->second == nullptr)
	{
		return 0.0;
	}

	const double ticksPerSecond = iter->second->GetTicksPerSecond();
	if (ticksPerSecond <= 0.0)
	{
		return 0.0;
	}

	return iter->second->GetDuration() / ticksPerSecond;
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
		for (UINT row = 0; row < 4; ++row)
		{
			outBoneTransformData.boneMats[idx].r[row] =
				DirectX::XMVectorLerp(from.boneMats[idx].r[row], to.boneMats[idx].r[row], blendRatio);
		}
	}
}

void Animator::SetIdentityBoneTransforms()
{
	for (DirectX::XMMATRIX& boneMat : _boneTransformData.boneMats)
	{
		boneMat = DirectX::XMMatrixIdentity();
	}
}
