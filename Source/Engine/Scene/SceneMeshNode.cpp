#include"../Msvc/EngineStd.h"
#include"SceneMeshNode.h"
#include"../Shader/ShaderManager.h"

unsigned int SceneMeshResourceLoader::VGetLoadedResourceSize(char* rawBuffer, unsigned int rawSize)
{
	return rawSize;
}

bool SceneMeshResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	Engine::Renderer renderer = Engine::GetRendererImpl();
	if (renderer == Engine::Renderer_D3D9)
	{
		GCC_ASSERT(0 && "This is not supported in D3D9");
	}
	else if (renderer == Engine::Renderer_D3D11)
	{
		shared_ptr<SceneMeshResourceExtraData> extra = shared_ptr<SceneMeshResourceExtraData>(GCC_NEW SceneMeshResourceExtraData());
		Assimp::Importer importer;
		const aiScene* scene=importer.ReadFileFromMemory(rawBuffer, rawSize, 
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_MakeLeftHanded);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			GCC_ERROR(importer.GetErrorString()); 
			return false;
		}
		extra->sceneModel.processNode(scene->mRootNode, scene);
		handle->SetExtra(extra);

		return true;
	}

	GCC_ASSERT(0 && "Unsupported Renderer in MeshResourceLoader::VLoadResource");
	return false;
}

void SceneModel::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		SceneMesh sceneMesh;
		processMesh(mesh, scene, sceneMesh);
		meshes.push_back(sceneMesh); //still exists copy cost...
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void SceneModel::processMesh(aiMesh* mesh, const aiScene* scene, SceneMesh& sceneMesh)
{
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
		}

		if (mesh->HasTangentsAndBitangents()) {
			vertex.tangent.x = mesh->mTangents[i].x;
			vertex.tangent.y = mesh->mTangents[i].y;
			vertex.tangent.z = mesh->mTangents[i].z;

			vertex.bitangent.x = mesh->mBitangents[i].x;
			vertex.bitangent.y = mesh->mBitangents[i].y;
			vertex.bitangent.z = mesh->mBitangents[i].z;
		}

		sceneMesh.vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			sceneMesh.indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex < 0) return;

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	aiTextureType aTypes[] = { aiTextureType_AMBIENT,aiTextureType_DIFFUSE,aiTextureType_SPECULAR,
		aiTextureType_NORMALS,aiTextureType_HEIGHT };
	TEXTURE_TYPE types[] = { TEXTURE_TYPE::ALBEDO,TEXTURE_TYPE::DIFFUSE,TEXTURE_TYPE::SPECULAR,
		TEXTURE_TYPE::NORMAL,TEXTURE_TYPE::HEIGHT };

	for (unsigned int t = 0; t < 5; t++)
	{
		TEXTURE_TYPE type = types[t];
		aiTextureType aType = aTypes[t];

		for (unsigned int i = 0; i < material->GetTextureCount(aType); i++)
		{
			aiString str;
			material->GetTexture(aType, i, &str);
			std::string name = std::string(TEXTTURE_TYPE_STR[t]) + "_" + std::to_string(i);
			defaultTextures.push_back(ShaderTexture(name, str.C_Str(), type));
		}
	}
}

//SceneMeshNode::SceneMeshNode(const std::string& _meshName) :meshName(_meshName){}

SceneMeshNode::SceneMeshNode(const std::string& _meshName, const std::string& _materialName, const std::string& _configName)
	:meshName(_meshName),materialName(_materialName),configName(_configName)
{
}

HRESULT SceneMeshNode::VOnRestore(Scene* pScene)
{

}