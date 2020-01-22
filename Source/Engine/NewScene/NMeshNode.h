#pragma once
#include"../Engine/Engine.h"
#include"NSceneNode.h"
#include"../Shader/ShaderResource.h"
#include"../Shader/MeshShader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT3 tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 bitangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT2 texCoords = XMFLOAT2(0.0f, 0.0f);
};

class SceneMesh
{
public:
	SceneMesh() = default;
	SceneMesh(const vector<Vertex>& _vertices, const vector<unsigned int>& _indices, const vector<ShaderTexture>& _textures)
		:vertices(_vertices), indices(_indices), textures(_textures) {}

	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<ShaderTexture> textures;
};

class SceneModel
{
	friend class SceneMeshResourceLoader;
public:
	vector<SceneMesh> meshes;
	vector<ShaderTexture> defaultTextures;

private:
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene, SceneMesh& sceneMesh);
};

class NMeshNode :public NSceneNode 
{
public:
	//SceneMeshNode(const string& _meshName);
	NMeshNode(const string& _meshName, const string& _materialName, const string& configName = "");

	virtual HRESULT VOnRestore(Scene* pScene);

protected:
	string meshName;
	string materialName;
	string configName;

	shared_ptr<MeshShader> shader;
	shared_ptr<MeshShaderInstance> instance;
};

class SceneMeshResourceExtraData : public IResourceExtraData
{
	friend class SceneMeshResourceLoader;

public:
	SceneMeshResourceExtraData() { };
	virtual ~SceneMeshResourceExtraData() { }
	virtual string VToString() { return "SceneMeshResourceExtraData"; }

	SceneModel sceneModel;
};

class SceneMeshResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char* rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual string VGetPattern() { return "*.fbx"; }
};