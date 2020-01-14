#include"EngineStd.h"
#include"ShaderManager.h"
#include"../Graphics3D/D3DRenderer.h"

ShaderManager* ShaderManager::instance = nullptr;

shared_ptr<MeshShaderInstance> ShaderManager::AddMetrial(shared_ptr<SceneShaderNode> node, const std::string& fileName)
{
	auto iter = shaderMap.find(fileName);
	if (iter != shaderMap.end()) {
		std::shared_ptr<MeshShader> shader = iter->second;
		if (shader->hasDefaultRes) {
			std::shared_ptr<MeshShaderInstance> instance=defaultInstanceMap.find(shader)->second;
			return instance;
		}
		else {
			return std::make_shared<MeshShaderInstance>(shader);
		}
	}
	else {
		ShaderComplier complier(fileName);
		complier.PreProcess();
		complier.Compile();
		shared_ptr<MeshShader> shader = std::make_shared<MeshShader>();;
		void* buff=nullptr;
		shader->hasDefaultRes = complier.HasDefaultRes(&buff);
		shader->shaderState = complier.GetShaderState();
		shader->shaderResourceMeta = complier.GetResourceMeta();
		shader->fileName = fileName;
		ComplieHLSLShader(shader, complier.GetVsCompiledString(), complier.GetPsCompiledString());
		shared_ptr<MeshShaderInstance> instance=std::make_shared<MeshShaderInstance>(shader);
		instance->buffData = buff;
		instance->textures = complier.GetShaderTextures();
		shaderMap.insert(std::make_pair(fileName, shader));
		instanceMap.insert(std::make_pair(node, instance));

		if (shader->hasDefaultRes) {
			defaultInstanceMap.insert(std::make_pair(shader,instance));
		}

		return instance;
	}
}

shared_ptr<MeshShaderInstance> ShaderManager::AddMetrial(shared_ptr<SceneShaderNode> node, const std::string& fileName, const std::string& configFileName)
{
	auto iter = shaderMap.find(fileName);
	if (iter != shaderMap.end()) {
		std::shared_ptr<MeshShader> shader = iter->second;
		shared_ptr<MeshShaderInstance> instance = std::make_shared<MeshShaderInstance>(shader);
		instance->shader = shader;

		ShaderResourceConfig config(configFileName, shader->shaderResourceMeta);
		instance->buffData = config.GetBuff();
		instance->textures = config.GetTextures();

		return instance;
	}
	else {
		ShaderComplier complier(fileName);
		complier.PreProcess();
		complier.Compile();
		shared_ptr<MeshShader> shader = std::make_shared<MeshShader>();;
		void* buff = nullptr;
		shader->hasDefaultRes = complier.HasDefaultRes(&buff);
		shader->shaderState = complier.GetShaderState();
		shader->shaderResourceMeta = complier.GetResourceMeta();
		shader->fileName = fileName;
		ComplieHLSLShader(shader, complier.GetVsCompiledString(), complier.GetPsCompiledString());
		shared_ptr<MeshShaderInstance> instance = std::make_shared<MeshShaderInstance>(shader);
		//Get From Config
		ShaderResourceConfig config(configFileName, shader->shaderResourceMeta);
		instance->buffData = config.GetBuff();
		instance->textures = config.GetTextures();

		shaderMap.insert(std::make_pair(fileName, shader));
		instanceMap.insert(std::make_pair(node, instance));

		return instance;
	}
}

HRESULT ShaderManager::ComplieHLSLShader(shared_ptr<MeshShader> shader,const std::string& vs,const std::string& ps)
{
	return ComplieVs(shader,vs)==S_OK && CompliePs(shader,ps)==S_OK;
}

HRESULT ShaderManager::ComplieVs(shared_ptr<MeshShader> shader,const std::string& vs)
{
	HRESULT hr = S_OK;
	ID3DBlob* pVertexShaderBuffer = NULL;
	if (FAILED(CompileShader(vs.c_str(), vs.size(), nullptr, shader->fileName.c_str(), "vs_4_0_level_9_1", &pVertexShaderBuffer)))
	{
		SAFE_RELEASE(pVertexShaderBuffer);
		return hr;
	}
	
	if (FAILED(DXUTGetD3D11Device()->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), NULL, &(shader->vertexShader))))
	{
		SAFE_RELEASE(pVertexShaderBuffer);
		return hr;
	}

	DXUT_SetDebugName(shader->vertexShader, shader->fileName);

	SetVertexLayOut(shader, pVertexShaderBuffer);
	SAFE_RELEASE(pVertexShaderBuffer);
	return S_OK;
}

HRESULT ShaderManager::SetVertexLayOut(shared_ptr<MeshShader> shader, ID3DBlob* pVertexShaderBuffer)
{
	HRESULT hr;

	if (SUCCEEDED(DXUTGetD3D11Device()->CreateInputLayout(D3D11VertexLayout, ARRAYSIZE(D3D11VertexLayout), pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), &(shader->vertexLayout11))))
	{
		DXUT_SetDebugName(shader->vertexLayout11, "Primary");

		// Setup constant buffers
		D3D11_BUFFER_DESC Desc;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Desc.MiscFlags = 0;

		Desc.ByteWidth = sizeof(cbMatrices);
		V_RETURN(DXUTGetD3D11Device()->CreateBuffer(&Desc, NULL, &(shader->cbMatrices)));
		DXUT_SetDebugName(shader->cbMatrices, "ConstantBuffer_Matrices");

		//Desc.ByteWidth = sizeof(ConstantBuffer_Lighting);
		//V_RETURN(DXUTGetD3D11Device()->CreateBuffer(&Desc, NULL, &m_pcbVSLighting));
		//DXUT_SetDebugName(m_pcbPSLighting, "ConstantBuffer_Lighting");

		Desc.ByteWidth = shader->shaderResourceMeta.GetStructSize();
		V_RETURN(DXUTGetD3D11Device()->CreateBuffer(&Desc, NULL, &(shader->cbCustom)));
		DXUT_SetDebugName(m_pcbVSMaterial, "ConstantBuffer_Material");
	}
}

HRESULT ShaderManager::CompliePs(shared_ptr<MeshShader> shader,const std::string& ps)
{
	HRESULT hr = S_OK;
	ID3DBlob* pPixelShaderBuffer = NULL;
	if (FAILED(CompileShader(ps.c_str(), ps.size(), nullptr, shader->fileName.c_str(), "ps_4_0_level_9_1", &pPixelShaderBuffer)))
	{
		SAFE_RELEASE(pPixelShaderBuffer);
		return hr;
	}

	if (FAILED(DXUTGetD3D11Device()->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(),
		pPixelShaderBuffer->GetBufferSize(), NULL, &(shader->pixelShader))))
	{
		SAFE_RELEASE(pPixelShaderBuffer);
		return hr;
	}
	

	SAFE_RELEASE(pPixelShaderBuffer);
	return hr;
}

HRESULT ShaderManager::CompileShader(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromMemory(pSrcData, SrcDataLen, pFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}
