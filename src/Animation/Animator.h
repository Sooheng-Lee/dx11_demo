#pragma once

class Animator
{
public:
	Animator();
	~Animator() = default;

	bool AddAnimation(const std::string& name, const std::shared_ptr<Animation>& animation);
	bool Play(const std::string& name, float blendDuration = 0.2f, bool restart = false);
	void Update(float deltaTime);

	bool HasAnimation(const std::string& name) const;
	const std::string& GetCurrentAnimationName() const { return _currentName; }
	double GetCurrentAnimationDurationSeconds() const;
	double GetAnimationDurationSeconds(const std::string& name) const;
	const BoneTransformData& GetBoneTransformData() const { return _boneTransformData; }

private:
	void SampleCurrent();
	void BlendBoneTransformData(
		const BoneTransformData& from,
		const BoneTransformData& to,
		float blendRatio,
		BoneTransformData& outBoneTransformData) const;
	void SetIdentityBoneTransforms();

private:
	std::unordered_map<std::string, std::shared_ptr<Animation>> _animations;

	std::string _currentName;
	std::string _nextName;
	double _currentTime = 0.0;
	double _nextTime = 0.0;

	float _blendDuration = 0.0f;
	float _blendElapsed = 0.0f;
	bool _isBlending = false;

	BoneTransformData _boneTransformData;
};
