#pragma once

#include"../Engine/Engine.h"
#include"../Scene/SceneShaderNode.h"
#include"MeshShader.h"
#include"ShaderComplier.h"

#include<map>
#include<string>
#include<memory>

using std::unique_ptr;

class ShaderManager 
{
	friend class SceneRender;
public:
	static ShaderManager* Get()
	{
		if (!instance)
			instance=GCC_NEW ShaderManager;
		return instance;
	}
	~ShaderManager() { if(instance) delete instance; }

	//RT has some problem... emmm fix later
	shared_ptr<MeshShaderInstance> AddMetrial(shared_ptr<SceneShaderNode> node,const std::string& fileName);
	shared_ptr<MeshShaderInstance> AddMetrial(shared_ptr<SceneShaderNode> node,const std::string& fileName,const std::string& configFileName);

private:
	HRESULT ComplieHLSLShader(shared_ptr<MeshShader> shader,const std::string& vs, const std::string& ps);
	HRESULT ComplieVs(shared_ptr<MeshShader> shader,const std::string& vs);
	HRESULT SetVertexLayOut(shared_ptr<MeshShader> shader, ID3DBlob* pVertexShaderBuffer);
	HRESULT CompliePs(shared_ptr<MeshShader> shader,const std::string& ps);

	HRESULT CompileShader(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

private:
	static ShaderManager* instance;
	ShaderManager();

	std::map<std::string,std::shared_ptr<MeshShader>> shaderMap;
	std::map<shared_ptr<SceneShaderNode>, shared_ptr<MeshShaderInstance>> instanceMap;
	std::map<std::shared_ptr<MeshShader>, std::shared_ptr<MeshShaderInstance>> defaultInstanceMap;
};
