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

private:
	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void SetVertexData(T& vertex, aiMesh* mesh, UINT idx);
private:
	std::vector<T> _vertices;
	std::vector<UINT> _indices;
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
		aiProcess_PreTransformVertices |
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
	for (UINT idx = 0; idx < mesh->mNumVertices; ++idx)
	{
		T vertex{};
		SetVertexData(vertex, mesh, idx);
		_vertices.push_back(vertex);
	}

	for (UINT idx = 0; idx < mesh->mNumFaces; ++idx)
	{
		const aiFace& face = mesh->mFaces[idx];
		for (UINT subIdx = 0; subIdx < face.mNumIndices; ++subIdx)
		{
			_indices.push_back(baseVertex + face.mIndices[subIdx]);
		}
	}
}

template<typename T>
inline void Model<T>::SetVertexData(T& vertex, aiMesh* mesh, UINT idx)
{
	if constexpr (std::is_same_v<T, VertexTexData>)
	{
		vertex.position = {
			mesh->mVertices[idx].x,
			mesh->mVertices[idx].y,
			mesh->mVertices[idx].z
		};

		// normal check
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
}
