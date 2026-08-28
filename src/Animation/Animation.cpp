#include "..\pch.h"

Animation::Animation()
{
	DirectX::XMStoreFloat4x4(&_globalInverseTransform, DirectX::XMMatrixIdentity());
	SetIdentityBoneTransforms(_boneTransformData);
}

bool Animation::Load(
	const std::string& filePath,
	const std::unordered_map<std::string, BoneInfo>& modelBoneInfoMap,
	UINT modelBoneCount)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals |
		aiProcess_FindInvalidData |
		aiProcess_ConvertToLeftHanded
	);

	if (scene == nullptr || scene->mRootNode == nullptr ||
		(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) ||
		scene->mNumAnimations == 0)
	{
		std::string errLog = std::string("Assimp Animation Load Failed ") + importer.GetErrorString();
		MessageBoxA(nullptr, errLog.c_str(), "Error", MB_ICONERROR);
		return false;
	}

	const aiAnimation* animation = scene->mAnimations[0];

	_duration = animation->mDuration;
	_ticksPerSecond = animation->mTicksPerSecond != 0.0 ? animation->mTicksPerSecond : 25.0;
	_currentTime = 0.0;
	_boneInfoMap = modelBoneInfoMap;
	_boneCount = modelBoneCount;
	_channels.clear();

	ReadHierarchyData(_rootNode, scene->mRootNode);
	ReadChannels(animation);

	DirectX::XMMATRIX rootTransform = DirectX::XMLoadFloat4x4(&_rootNode.transform);
	DirectX::XMMATRIX globalInverseTransform = DirectX::XMMatrixInverse(nullptr, rootTransform);
	DirectX::XMStoreFloat4x4(&_globalInverseTransform, globalInverseTransform);

	SetIdentityBoneTransforms(_boneTransformData);
	CalculateBoneTransform(_rootNode, DirectX::XMMatrixIdentity(), _currentTime, _boneTransformData);

	return true;
}

void Animation::Update(float deltaTime)
{
	_currentTime = AdvanceTime(_currentTime, deltaTime);
	Sample(_currentTime, _boneTransformData);
}

void Animation::Reset()
{
	_currentTime = 0.0;
	Sample(_currentTime, _boneTransformData);
}

double Animation::AdvanceTime(double animationTime, float deltaTime) const
{
	if (_duration <= 0.0)
	{
		return 0.0;
	}

	animationTime += static_cast<double>(deltaTime) * _ticksPerSecond;
	animationTime = std::fmod(animationTime, _duration);

	if (animationTime < 0.0)
	{
		animationTime += _duration;
	}

	return animationTime;
}

void Animation::Sample(double animationTime, BoneTransformData& outBoneTransformData) const
{
	SetIdentityBoneTransforms(outBoneTransformData);

	if (_duration <= 0.0)
	{
		return;
	}

	animationTime = std::fmod(animationTime, _duration);
	if (animationTime < 0.0)
	{
		animationTime += _duration;
	}

	CalculateBoneTransform(_rootNode, DirectX::XMMatrixIdentity(), animationTime, outBoneTransformData);
}

void Animation::ReadHierarchyData(AnimationNodeData& dest, const aiNode* src)
{
	dest.name = src->mName.C_Str();
	dest.transform = ConvertMatrix(src->mTransformation);
	dest.children.clear();
	dest.children.resize(src->mNumChildren);

	for (UINT idx = 0; idx < src->mNumChildren; ++idx)
	{
		ReadHierarchyData(dest.children[idx], src->mChildren[idx]);
	}
}

void Animation::ReadChannels(const aiAnimation* animation)
{
	for (UINT idx = 0; idx < animation->mNumChannels; ++idx)
	{
		const aiNodeAnim* srcChannel = animation->mChannels[idx];
		NodeAnimChannel channel;
		channel.name = srcChannel->mNodeName.C_Str();
		channel.positions.reserve(srcChannel->mNumPositionKeys);
		channel.rotations.reserve(srcChannel->mNumRotationKeys);
		channel.scales.reserve(srcChannel->mNumScalingKeys);

		for (UINT keyIdx = 0; keyIdx < srcChannel->mNumPositionKeys; ++keyIdx)
		{
			const aiVectorKey& key = srcChannel->mPositionKeys[keyIdx];
			channel.positions.push_back({
				DirectX::XMFLOAT3(key.mValue.x, key.mValue.y, key.mValue.z),
				key.mTime
			});
		}

		for (UINT keyIdx = 0; keyIdx < srcChannel->mNumRotationKeys; ++keyIdx)
		{
			const aiQuatKey& key = srcChannel->mRotationKeys[keyIdx];
			channel.rotations.push_back({
				DirectX::XMFLOAT4(key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w),
				key.mTime
			});
		}

		for (UINT keyIdx = 0; keyIdx < srcChannel->mNumScalingKeys; ++keyIdx)
		{
			const aiVectorKey& key = srcChannel->mScalingKeys[keyIdx];
			channel.scales.push_back({
				DirectX::XMFLOAT3(key.mValue.x, key.mValue.y, key.mValue.z),
				key.mTime
			});
		}

		_channels[channel.name] = channel;
	}
}

void Animation::CalculateBoneTransform(
	const AnimationNodeData& node,
	const DirectX::XMMATRIX& parentTransform,
	double animationTime,
	BoneTransformData& outBoneTransformData) const
{
	DirectX::XMMATRIX nodeTransform = DirectX::XMLoadFloat4x4(&node.transform);
	const NodeAnimChannel* channel = FindChannel(node.name);

	if (channel != nullptr)
	{
		nodeTransform = channel->GetLocalTransform(animationTime);
	}

	DirectX::XMMATRIX globalTransform = nodeTransform * parentTransform;
	auto boneIter = _boneInfoMap.find(node.name);

	if (boneIter != _boneInfoMap.end() && boneIter->second.id < _boneCount && boneIter->second.id < 128)
	{
		const BoneInfo& boneInfo = boneIter->second;
		DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&boneInfo.offsetMatrix);
		DirectX::XMMATRIX globalInverseTransform = DirectX::XMLoadFloat4x4(&_globalInverseTransform);
		DirectX::XMMATRIX finalTransform = offsetTransform * globalTransform * globalInverseTransform;

		outBoneTransformData.boneMats[boneInfo.id] = DirectX::XMMatrixTranspose(finalTransform);
	}

	for (const AnimationNodeData& child : node.children)
	{
		CalculateBoneTransform(child, globalTransform, animationTime, outBoneTransformData);
	}
}

const NodeAnimChannel* Animation::FindChannel(const std::string& nodeName) const
{
	auto iter = _channels.find(nodeName);
	if (iter == _channels.end())
	{
		return nullptr;
	}

	return &iter->second;
}

void Animation::SetIdentityBoneTransforms(BoneTransformData& outBoneTransformData)
{
	for (DirectX::XMMATRIX& boneMat : outBoneTransformData.boneMats)
	{
		boneMat = DirectX::XMMatrixIdentity();
	}
}

DirectX::XMMATRIX NodeAnimChannel::GetLocalTransform(double animationTime) const
{
	DirectX::XMMATRIX scale = InterpolateScale(animationTime);
	DirectX::XMMATRIX rotation = InterpolateRotation(animationTime);
	DirectX::XMMATRIX translation = InterpolatePosition(animationTime);

	return scale * rotation * translation;
}

DirectX::XMMATRIX NodeAnimChannel::InterpolatePosition(double animationTime) const
{
	if (positions.empty())
	{
		return DirectX::XMMatrixIdentity();
	}

	if (positions.size() == 1)
	{
		const DirectX::XMFLOAT3& value = positions[0].value;
		return DirectX::XMMatrixTranslation(value.x, value.y, value.z);
	}

	UINT index = GetPositionIndex(animationTime);
	UINT nextIndex = index + 1;
	float scaleFactor = GetScaleFactor(positions[index].time, positions[nextIndex].time, animationTime);

	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&positions[index].value);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&positions[nextIndex].value);
	DirectX::XMVECTOR interpolated = DirectX::XMVectorLerp(start, end, scaleFactor);
	DirectX::XMFLOAT3 value;
	DirectX::XMStoreFloat3(&value, interpolated);

	return DirectX::XMMatrixTranslation(value.x, value.y, value.z);
}

DirectX::XMMATRIX NodeAnimChannel::InterpolateRotation(double animationTime) const
{
	if (rotations.empty())
	{
		return DirectX::XMMatrixIdentity();
	}

	if (rotations.size() == 1)
	{
		DirectX::XMVECTOR rotation = DirectX::XMLoadFloat4(&rotations[0].value);
		rotation = DirectX::XMQuaternionNormalize(rotation);
		return DirectX::XMMatrixRotationQuaternion(rotation);
	}

	UINT index = GetRotationIndex(animationTime);
	UINT nextIndex = index + 1;
	float scaleFactor = GetScaleFactor(rotations[index].time, rotations[nextIndex].time, animationTime);

	DirectX::XMVECTOR start = DirectX::XMLoadFloat4(&rotations[index].value);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat4(&rotations[nextIndex].value);
	DirectX::XMVECTOR interpolated = DirectX::XMQuaternionSlerp(start, end, scaleFactor);
	interpolated = DirectX::XMQuaternionNormalize(interpolated);

	return DirectX::XMMatrixRotationQuaternion(interpolated);
}

DirectX::XMMATRIX NodeAnimChannel::InterpolateScale(double animationTime) const
{
	if (scales.empty())
	{
		return DirectX::XMMatrixIdentity();
	}

	if (scales.size() == 1)
	{
		const DirectX::XMFLOAT3& value = scales[0].value;
		return DirectX::XMMatrixScaling(value.x, value.y, value.z);
	}

	UINT index = GetScaleIndex(animationTime);
	UINT nextIndex = index + 1;
	float scaleFactor = GetScaleFactor(scales[index].time, scales[nextIndex].time, animationTime);

	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&scales[index].value);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&scales[nextIndex].value);
	DirectX::XMVECTOR interpolated = DirectX::XMVectorLerp(start, end, scaleFactor);
	DirectX::XMFLOAT3 value;
	DirectX::XMStoreFloat3(&value, interpolated);

	return DirectX::XMMatrixScaling(value.x, value.y, value.z);
}

UINT NodeAnimChannel::GetPositionIndex(double animationTime) const
{
	for (UINT idx = 0; idx < positions.size() - 1; ++idx)
	{
		if (animationTime < positions[idx + 1].time)
		{
			return idx;
		}
	}

	return static_cast<UINT>(positions.size() - 2);
}

UINT NodeAnimChannel::GetRotationIndex(double animationTime) const
{
	for (UINT idx = 0; idx < rotations.size() - 1; ++idx)
	{
		if (animationTime < rotations[idx + 1].time)
		{
			return idx;
		}
	}

	return static_cast<UINT>(rotations.size() - 2);
}

UINT NodeAnimChannel::GetScaleIndex(double animationTime) const
{
	for (UINT idx = 0; idx < scales.size() - 1; ++idx)
	{
		if (animationTime < scales[idx + 1].time)
		{
			return idx;
		}
	}

	return static_cast<UINT>(scales.size() - 2);
}

float NodeAnimChannel::GetScaleFactor(double lastTime, double nextTime, double animationTime) const
{
	double frameDelta = nextTime - lastTime;
	if (frameDelta <= 0.0)
	{
		return 0.0f;
	}

	return std::clamp(static_cast<float>((animationTime - lastTime) / frameDelta), 0.0f, 1.0f);
}
