#pragma once
#include"../Engine/Engine.h"
#include"Scene.h"
#include"SceneShaderNode.h"
#include"../Shader/ShaderResource.h"
#include"../Shader/MeshShader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>

struct Vertex 
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT3 tangent=XMFLOAT3(0.0f,0.0f, 0.0f);
	XMFLOAT3 bitangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT2 texCoords=XMFLOAT2(0.0f,0.0f);
};

class SceneMesh 
{
public:
	SceneMesh() = default;
	SceneMesh(const std::vector<Vertex>& _vertices,const std::vector<unsigned int>& _indices,const std::vector<ShaderTexture>& _textures) 
	:vertices(_vertices),indices(_indices),textures(_textures){}

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<ShaderTexture> textures;
};

class SceneModel 
{
	friend class SceneMeshResourceLoader;
public:
	std::vector<SceneMesh> meshes;
	std::vector<ShaderTexture> defaultTextures;

private:
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene, SceneMesh& sceneMesh);

};

class SceneMeshResourceExtraData : public IResourceExtraData
{
	friend class SceneMeshResourceLoader;

public:
	SceneMeshResourceExtraData() { };
	virtual ~SceneMeshResourceExtraData() { }
	virtual std::string VToString() { return "SceneMeshResourceExtraData"; }

	SceneModel sceneModel;
};

class SceneMeshResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char* rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.fbx"; }
};

class SceneMeshNode:public SceneShaderNode
{
public:
	//SceneMeshNode(const std::string& _meshName);
	SceneMeshNode(const std::string& _meshName, const std::string& _materialName,const std::string& configName="");
	
	virtual HRESULT VOnRestore(Scene* pScene);
	
protected:
	std::string meshName;
	std::string materialName;
	std::string configName;

	shared_ptr<MeshShader> shader;
	shared_ptr<MeshShaderInstance> instance;
};
