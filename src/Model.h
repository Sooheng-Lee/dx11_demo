#pragma once

template <typename T>
class Model
{
public:
	Model() = default;
	~Model() = default;

public:
	bool Load(const std::string& filePath);
	const std::vector<T>& GetVertices() {
		return _vertices;
	};

	const std::vector<UINT>& GetIndices()
	{
		return _indices;
	};

	const std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap() const
	{
		return _boneInfoMap;
	}

	UINT GetBoneCount() const
	{
		return _boneCount;
	}

private:
	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void SetVertexData(T& vertex, aiMesh* mesh, UINT idx);
	template <typename TVertex>
	void SetMeshVertexData(TVertex& vertex, aiMesh* mesh, UINT idx);
	void ExtractBoneWeight(aiMesh* mesh, UINT baseVertex);
private:
	std::vector<T> _vertices;
	std::vector<UINT> _indices;
	std::unordered_map<std::string, BoneInfo> _boneInfoMap;
	UINT _boneCount = 0;
};

template<typename T>
inline bool Model<T>::Load(const std::string& filePath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_FindDegenerates |
		aiProcess_FindInvalidData |
		aiProcess_ImproveCacheLocality |
		aiProcess_ConvertToLeftHanded
	);
	if (scene == nullptr || scene->mRootNode == nullptr ||
		(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
	{
		std::string errLog = std::string("Assimp Load Failed ") + importer.GetErrorString();
		MessageBoxA(nullptr, errLog.c_str(), "Error", MB_ICONERROR);
		return false;
	}

	_vertices.clear();
	_indices.clear();

	ProcessNode(scene->mRootNode, scene);

	return true;
}

template<typename T>
inline void Model<T>::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (UINT idx = 0; idx < node->mNumMeshes; ++idx)
	{
		UINT meshIdx = node->mMeshes[idx];
		aiMesh* mesh = scene->mMeshes[meshIdx];
		ProcessMesh(mesh, scene);
	}
	for (UINT idx = 0; idx < node->mNumChildren; ++idx)
	{
		ProcessNode(node->mChildren[idx], scene);
	}

}

template<typename T>
inline void Model<T>::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	const UINT baseVertex = static_cast<UINT>(_vertices.size());
	// Vertex
	for (UINT idx = 0; idx < mesh->mNumVertices; ++idx)
	{
		T vertex{};
		SetVertexData(vertex, mesh, idx);
		_vertices.push_back(vertex);
	}

	// Index
	for (UINT idx = 0; idx < mesh->mNumFaces; ++idx)
	{
		const aiFace& face = mesh->mFaces[idx];
		for (UINT subIdx = 0; subIdx < face.mNumIndices; ++subIdx)
		{
			_indices.push_back(baseVertex + face.mIndices[subIdx]);
		}
	}

	// Bone
	if constexpr (std::is_same_v<T, VertexAnimData>)
	{
		ExtractBoneWeight(mesh, baseVertex);
	}
}

template<typename T>
inline void Model<T>::SetVertexData(T& vertex, aiMesh* mesh, UINT idx)
{
	if constexpr (std::is_same_v<T, VertexTexData>)
	{
		SetMeshVertexData(vertex, mesh, idx);
	}
	else if constexpr (std::is_same_v<T, VertexAnimData>)
	{
		SetMeshVertexData(vertex, mesh, idx);
	}
}

template<typename T>
template<typename TVertex>
inline void Model<T>::SetMeshVertexData(TVertex& vertex, aiMesh* mesh, UINT idx)
{
	vertex.position = {
		mesh->mVertices[idx].x,
		mesh->mVertices[idx].y,
		mesh->mVertices[idx].z
	};

	if (mesh->HasNormals())
	{
		vertex.normal =
		{
			mesh->mNormals[idx].x,
			mesh->mNormals[idx].y,
			mesh->mNormals[idx].z
		};
	}
	else
	{
		vertex.normal =
		{
			0.0f,
			0.0f,
			0.0f
		};
	}

	if (mesh->mTextureCoords[0] != nullptr)
	{
		vertex.uv =
		{
			mesh->mTextureCoords[0][idx].x,
			mesh->mTextureCoords[0][idx].y
		};
	}
	else
	{
		vertex.uv =
		{
			0.0f,
			0.0f
		};
	}
}
template<typename T>
inline void Model<T>::ExtractBoneWeight(aiMesh* mesh, UINT baseVertex)
{
	if constexpr (!std::is_same_v<T, VertexAnimData>) return;

	for (UINT boneIdx = 0; boneIdx < mesh->mNumBones; ++boneIdx)
	{
		aiBone* bone = mesh->mBones[boneIdx];
		std::string boneName = bone->mName.C_Str();
		UINT boneID = 0;
		// 처음 발견된 Bone
		auto iter = _boneInfoMap.find(boneName);
		if (iter == _boneInfoMap.end())
		{
			BoneInfo boneInfo;
			boneInfo.id = static_cast<int>(_boneCount);
			boneInfo.offsetMatrix = ConvertMatrix(bone->mOffsetMatrix);
			_boneInfoMap[boneName] = boneInfo;

			boneID = _boneCount;
			++_boneCount;
		}
		else
		{
			boneID = static_cast<UINT>(iter->second.id);
		}

		// Vertex Weight
		for (UINT weightIdx = 0; weightIdx < bone->mNumWeights; ++weightIdx)
		{
			const aiVertexWeight& weight = bone->mWeights[weightIdx];
			UINT vertexIndex = baseVertex + weight.mVertexId;

			if (vertexIndex >= _vertices.size())
			{
				continue;
			}

				_vertices[vertexIndex].AddBoneData(boneID,weight.mWeight);
		}
	}
}
