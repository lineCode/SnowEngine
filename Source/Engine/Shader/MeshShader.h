#pragma once
#include"../Script/LuaStateManager.h"
#include"ShaderResource.h"
#include"ShaderComplier.h"

#include<string>
#include<map>
#include<set>

class MeshShader {
public:
	MeshShader();
	MeshShader(const char* fileName,int shaderModel=1);
	~MeshShader();

	//Init Shader 
	void InitShaderEnv();

public:
	bool hasDefaultRes = false;
	ShaderState shaderState;
	ShaderResourceMeta shaderResourceMeta;

protected:
	ID3D11InputLayout* vertexLayout11;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;

	HRESULT SetTexture(const std::string& textureName);
	HRESULT SetTexture(ID3D11ShaderResourceView* const* pDiffuseRV, ID3D11SamplerState* const* ppSamplers);
};

struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectPass;

class MeshShaderInstance {
public:
	MeshShaderInstance(shared_ptr<MeshShader> _shader):shader(_shader) {}
	~MeshShaderInstance() { if(buffData) delete buffData; }

	shared_ptr<MeshShader> shader;
	void* buffData = nullptr;
	std::vector<ShaderTexture> textures;

	void Set(shared_ptr<MeshShader> _shader, void* _buffData, std::vector<ShaderTexture> _textures);
};

void MeshShaderInstance::Set(shared_ptr<MeshShader> _shader, void* _buffData, std::vector<ShaderTexture> _textures)
{
	shader = _shader;
	buffData = _buffData;
	textures = _textures;
}
