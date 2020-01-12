#pragma once
#include"../Script/StandAloneLuaEnv.h"
#include"ShaderResource.h"

#include<string>
#include<map>
#include<set>

using LObject = LuaPlus::LuaObject;
using std::string;

class ShaderComplier {
public:
	ShaderComplier();
	ShaderComplier(const string& fileName, int shaderModel = 1);
	~ShaderComplier();

	void PreProcess();
	bool Compile();

	std::string GetVsCompiledString() { return vsOutput; }
	std::string GetPsCompiledString() { return psOutput; }

	ShaderResourceMeta GetResourceMeta();

	bool HasDefaultRes(void** buff);
	std::vector<ShaderTexture> GetShaderTextures();
	ShaderState GetShaderState();

	//Helper Functions
private:
	void InitEnv();
	std::string GenerateVSContantBufferCode();
	std::string GeneratePSShaderResourceCode();

	//Shader Config
private:
	StandAloneLuaEnv scriptEnv;

	std::string fileName;
	//Shader Model 0==OpenGL 1==DX11 
	int shaderMode = 1;
	//Preprocess LuaCode
	std::string luaCode;
	//Complied Shader Code
	std::string vsOutput;
	std::string psOutput;
	//Complier Used Variables 

	//Output Info
	ShaderResourceMeta resourceMeta;
	ShaderState shaderState;

	bool hasDefaultRes;
	//if hasDefaultRes The Following Two Params will be vaild
	void* buffer;
	std::vector<ShaderTexture> shaderTextures;

private:
	int GetNewId() { static int count = 0; return ++count; }
	enum class Mode { config, vs, ps };
	Mode mode = Mode::config;
	//Resource
	std::set<std::string> sampleStates;
	std::set<std::string> textures;

	//VS PS
	LObject vsFunc;
	LObject psFunc;
	std::vector<std::string> vsCodeLines;
	std::map<std::string, std::string> interParams; //name-type
	std::vector<std::string> psCodeLines;

	//Interface between Lua And C++
private:
	//Configuration
	void SetVsShader(LObject _vsFunc) { vsFunc = _vsFunc; }
	void SetPsShader(LObject _psFunc) { psFunc = _psFunc; }
	void SetRenderQueue(int _queue);
	void CreateSampleState(const char* sampleStateName, int _filter = 0, int _u = 0, int _v = 0);
	void AddTexture(const char* textureName);
	void AddTextureA(const char* textureName, const char* textureDir);
	void AddFloat(const char* name);
	void AddFloatA(const char* name, float x);
	void AddFloat2(const char* name);
	void AddFloat2A(const char* name, float x, float y);
	void AddFloat3(const char* name);
	void AddFloat3A(const char* name, float x, float y, float z);
	void AddFloat4(const char* name);
	void AddFloat4A(const char* name, float x, float y, float z, float w);
	void AddMat3x3(const char* name);
	void AddMat4x4(const char* name);
	void UseAo(bool useAo);
	void UseShadow(bool useShadow);
	//Global
	LuaPlus::LuaObject GetVal(LObject);
	LuaPlus::LuaObject GetMatrix(const char* key);
	LuaPlus::LuaObject GetVsIn(const char* key);
	void SetVsOut(const char* key, LObject lo);
	LuaPlus::LuaObject GetPsIn(const char* key);

	//Shader Type
	LObject GetFloat(LObject x);
	LObject GetFloat2(LObject x, LObject y);
	LObject GetFloat3(LObject x, LObject y, LObject z);
	LObject GetFloat4(LObject x, LObject y, LObject z, LObject w);

	LObject  OperatorReload(LObject& l1, LObject& l2, const char* operators, const char* metaTable);

	LObject floatmul(LObject l1, LObject l2);
	LObject floatadd(LObject l1, LObject l2);
	LObject floatsub(LObject l1, LObject l2);
	LObject floatdiv(LObject l1, LObject l2);

	LObject float2mul(LObject l1, LObject l2);
	LObject float2add(LObject l1, LObject l2);
	LObject float2sub(LObject l1, LObject l2);
	LObject float2div(LObject l1, LObject l2);

	LObject float3mul(LObject l1, LObject l2);
	LObject float3add(LObject l1, LObject l2);
	LObject float3sub(LObject l1, LObject l2);
	LObject float3div(LObject l1, LObject l2);

	LObject float4mul(LObject l1, LObject l2);
	LObject float4add(LObject l1, LObject l2);
	LObject float4sub(LObject l1, LObject l2);
	LObject float4div(LObject l1, LObject l2);

	//Shader Functions
	LObject GetMul(LObject l1, LObject l2);
	LObject GetDot(LObject l1, LObject l2);
	LObject Sample(const char* textureName, LObject uv, const char* sampleState);
	LObject Normalize(LObject l1);
};

//Get Instance Data From  Config File
class ShaderResourceConfig {
public:
	ShaderResourceConfig(std::string _fileName, ShaderResourceMeta _meta);

	~ShaderResourceConfig();

	void* GetBuff();

	std::vector<ShaderTexture> GetTextures();

private:
	std::string configFileName;
	ShaderResourceMeta meta;
	void* buffer;
	std::vector<ShaderTexture> textures;
	StandAloneLuaEnv scriptEnv;

private:
	void InitEnv();
	bool LoadAndParse();

	XMFLOAT4 temp;
private:
	//bind functions
	void Float2(float x, float y);
	void Float3(float x, float y, float z);
	void Float4(float x, float y, float z, float w);
};