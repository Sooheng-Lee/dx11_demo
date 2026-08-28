#pragma once

struct NodeAnimKeyPosition
{
	DirectX::XMFLOAT3 value;
	double time = 0.0;
};

struct NodeAnimKeyRotation
{
	DirectX::XMFLOAT4 value;
	double time = 0.0;
};

struct NodeAnimKeyScale
{
	DirectX::XMFLOAT3 value;
	double time = 0.0;
};

struct NodeAnimChannel
{
	std::string name;
	std::vector<NodeAnimKeyPosition> positions;
	std::vector<NodeAnimKeyRotation> rotations;
	std::vector<NodeAnimKeyScale> scales;

	DirectX::XMMATRIX GetLocalTransform(double animationTime) const;

private:
	DirectX::XMMATRIX InterpolatePosition(double animationTime) const;
	DirectX::XMMATRIX InterpolateRotation(double animationTime) const;
	DirectX::XMMATRIX InterpolateScale(double animationTime) const;
	UINT GetPositionIndex(double animationTime) const;
	UINT GetRotationIndex(double animationTime) const;
	UINT GetScaleIndex(double animationTime) const;
	float GetScaleFactor(double lastTime, double nextTime, double animationTime) const;
};

struct AnimationNodeData
{
	std::string name;
	DirectX::XMFLOAT4X4 transform;
	std::vector<AnimationNodeData> children;
};

class Animation
{
public:
	Animation();
	~Animation() = default;

	bool Load(
		const std::string& filePath,
		const std::unordered_map<std::string, BoneInfo>& modelBoneInfoMap,
		UINT modelBoneCount);

	void Update(float deltaTime);
	void Reset();

	const BoneTransformData& GetBoneTransformData() const
	{
		return _boneTransformData;
	}

private:
	void ReadHierarchyData(AnimationNodeData& dest, const aiNode* src);
	void ReadChannels(const aiAnimation* animation);
	void CalculateBoneTransform(const AnimationNodeData& node, const DirectX::XMMATRIX& parentTransform);
	const NodeAnimChannel* FindChannel(const std::string& nodeName) const;
	void SetIdentityBoneTransforms();

private:
	double _duration = 0.0;
	double _ticksPerSecond = 1.0;
	double _currentTime = 0.0;
	DirectX::XMFLOAT4X4 _globalInverseTransform;
	AnimationNodeData _rootNode;
	std::unordered_map<std::string, NodeAnimChannel> _channels;
	std::unordered_map<std::string, BoneInfo> _boneInfoMap;
	UINT _boneCount = 0;
	BoneTransformData _boneTransformData;
};
