#define _CRT_SECURE_NO_WARNINGS
#define QuickRegister(A) scriptEnv.RegisterFunction(#A,*this,&ShaderComplier::A)

#include"../Msvc/EngineStd.h"
#include"ShaderComplier.h"

#pragma region Construct
ShaderComplier::ShaderComplier()
{
	InitEnv();
}

ShaderComplier::ShaderComplier(const string& _fileName, int _shaderModel) :ShaderComplier()
{
	this->fileName = _fileName;
	this->shaderMode = _shaderModel;
}

ShaderComplier::~ShaderComplier()
{}

#pragma endregion

#pragma region Core
void ShaderComplier::InitEnv()
{
	QuickRegister(SetVsShader);
	QuickRegister(SetPsShader);

	QuickRegister(SetRenderQueue);
	QuickRegister(CreateSampleState);
	QuickRegister(AddTexture);
	QuickRegister(AddTextureA);
	QuickRegister(AddFloat);
	QuickRegister(AddFloatA);
	QuickRegister(AddFloat2);
	QuickRegister(AddFloat2A);
	QuickRegister(AddFloat3);
	QuickRegister(AddFloat4);
	QuickRegister(AddFloat4A);
	QuickRegister(UseAo);
	QuickRegister(UseShadow);

	QuickRegister(GetMatrix);
	QuickRegister(GetVsIn);
	QuickRegister(SetVsOut);
	QuickRegister(GetPsIn);
	QuickRegister(GetVal);

	QuickRegister(GetFloat);
	QuickRegister(GetFloat);
	QuickRegister(GetFloat2);
	QuickRegister(GetFloat3);
	QuickRegister(GetFloat4);

	QuickRegister(floatmul);
	QuickRegister(floatadd);
	QuickRegister(floatsub);
	QuickRegister(floatdiv);

	QuickRegister(float2mul);
	QuickRegister(float2add);
	QuickRegister(float2sub);
	QuickRegister(float2div);

	QuickRegister(float3mul);
	QuickRegister(float3add);
	QuickRegister(float3sub);
	QuickRegister(float3div);

	QuickRegister(float4mul);
	QuickRegister(float4add);
	QuickRegister(float4sub);
	QuickRegister(float4div);

	QuickRegister(GetMul);
	QuickRegister(GetDot);
	QuickRegister(Sample);
	QuickRegister(Normalize);

	scriptEnv.LoadAndExecuteScriptResource("Shader\\shader.lua");
}

void ShaderComplier::PreProcess()
{
	shared_ptr<ResHandle> pResourceHandle = scriptEnv.LoadScriptResource(fileName.c_str());
	luaCode = pResourceHandle->Buffer();

	//handle all assign statments
	bool inScope = false;
	for (unsigned int i = 0; i < luaCode.size(); i++) {
		char c = luaCode[i];
		if (c == '{') inScope = true;
		if (c == '}') inScope = false;
		if (!inScope && c == '=' && (i + 1) < luaCode.size() && luaCode[(i + 1)] != '=') {
			luaCode.insert((i + 1), "(");
			int insertI = -1;
			for (unsigned int j = i + 2; j < luaCode.size(); j++) {
				if (luaCode[j] == ';') {
					insertI = j;
					break;
				}
			}
			if (insertI != -1) {
				luaCode.insert(insertI, ").val");
			}
		}
	}
}

string ShaderComplier::GenerateVSContantBufferCode()
{
	//Generate Common Used Buffer
	string commonBuffer = "cbuffer cbMatrices : register( b0 )\n{\n"
		"float4x4		worldviewproj : packoffset(c0);\n"
		"float4x4		invworldviewproj : packoffset(c0);\n"
		"float4x4		world : packoffset(c4);\n"
		"float4x4		view : packoffset(c4);\n"
		"float4x4		proj : packoffset(c4);\n"
		"float4x4		invworld : packoffset(c4);\n"
		"float4x4		invview : packoffset(c4);\n"
		"float4x4		invproj : packoffset(c4);\n"
		"float4			view_position : packoffset(c4);\n"
		"float4			position : packoffset(c4);\n"
		"float			nearz;\n"
		"float			farz;\n"
		"float			fov;\n"
		"float			time\n"	
		"}; ";

	//Generate Custom Shader Buffer
	string customBuffer = "cbuffer cbCustom:register( b1 )\n{\n";
	resourceMeta.GetStructSize();
	const std::vector<ShaderResourceItem> metaItems = resourceMeta.GetItems();
	for (auto iter = metaItems.begin(); iter != metaItems.end(); iter++) {
		string type;
		switch (iter->type) {
		case ShaderResourceType::FLOAT: type = "float"; break;
		case ShaderResourceType::FLOAT2: type = "float2"; break;
		case ShaderResourceType::FLOAT3: type = "float3"; break;
		case ShaderResourceType::FLOAT4: type = "float4"; break;
		case ShaderResourceType::MAT4x4: type = "float4x4"; break;
		case ShaderResourceType::MAT3x3: type = "float3x3"; break;
		default:break;
		}
		customBuffer += (type + "  " + iter->name + " ;\n");
	}
	customBuffer+="};";
}

string ShaderComplier::GeneratePSShaderResourceCode() 
{
	string textureCode;
	int index = 0;
	for (auto iter = textures.begin(); iter != textures.end(); iter++) {
		//We assume that its enough to store all textures in register(t0) which can contain 128 textures
		textureCode = textureCode + "Texture2D " + *iter + " :register(t0)" + "\n";
	}

	//Custom Config
	if (shaderState.useAo) {
		textureCode = textureCode + "Texture2D "+"ao_map"+ " :register(t1)" + "\n";
	}
	if (shaderState.useShadow) {
		textureCode = textureCode + "Texture2D " + "shadow_map" + " :register(t1)" + "\n";
	}

	//Sample State
	string sampleStateCode;
	index = 0;
	for (auto iter = sampleStates.begin(); iter != sampleStates.end(); iter++) {
		//We assume that its enough to store all sampleStates in register(s0)
		sampleStateCode = sampleStateCode + "SampleState " + *iter + " :register(s0)" + "\n";
		index++;
	}
	return textureCode + "\n" + sampleStateCode;
}

bool ShaderComplier::ParseCustomConfig(LObject& customConfigFunc)
{
	if (customConfigFunc.IsFunction() == false)
		return false;
	LuaPlus::LuaFunction<LObject> configfunc(customConfigFunc);
	LObject customFuncTable = configfunc();
	if (customFuncTable.IsTable()) {
		//to be continue...
	}
	std::vector<ShaderResourceItem> items=resourceMeta.GetItems();
	for (auto iter = items.begin(); iter != items.end(); iter++) {
		LObject globalsObj = scriptEnv.GetState()->GetGlobals();
		LObject table = globalsObj.CreateTable(iter->name.c_str());
		table.SetString("name", iter->name.c_str());
		LObject getValueMetatable = scriptEnv.GetState()->GetGlobals().GetByName("getvalue_metatable");
		table.SetMetatable(getValueMetatable);

		switch (iter->type) {
		case ShaderResourceType::FLOAT2 : {
			table.SetString("type", "float2");
		}break;
		case ShaderResourceType::FLOAT3 : {
			table.SetString("type", "float3");
		}break;
		case ShaderResourceType::FLOAT4: {
			table.SetString("type", "float4");
		}break;
		case ShaderResourceType::MAT3x3: {
			table.SetString("type", "float3x3");
		}break;
		case ShaderResourceType::MAT4x4: {
			table.SetString("type", "float4x4");
		}break;
		default:break;
		}
	}
}

bool ShaderComplier::Compile()
{
	if (fileName.empty()) return false;
	scriptEnv.ExecuteString(luaCode.c_str());

	LObject configFunc = scriptEnv.GetState()->GetGlobals().GetByName("config");
	if (configFunc.IsNil()) return false;

	LuaPlus::LuaFunction<LObject> func(configFunc);
	LObject configuration = func();
	if (!configuration.IsTable()) return false;

	LObject lightModel = configuration.GetByName("lightmodel");

	if (!strcmp(lightModel.GetString(), "custom")) {
		auto customConfigFunc = configuration.GetByName("custom");
		ParseCustomConfig(customConfigFunc);

		//generate vs hlsl shader
		if (!vsFunc.IsNil()) {
			mode = Mode::vs;
			LuaPlus::LuaFunction<void*> func(vsFunc);
			func();
		}
		//generate ps hlsl shader
		LObject psReturnObj;
		if (!psFunc.IsNil()) {
			mode = Mode::ps;
			LuaPlus::LuaFunction<LObject> func(psFunc);
			psReturnObj = func();
		}
		//generate vs
		string vsBufferStrut=GenerateVSContantBufferCode();
		string vsStructIn = "struct VertexIn\n{ \n  float3 positon:POSITION;\n"
			"  float3 normal:NORMAL\n"
			"  float2 texcord:TEXCORD\n"
			"}\n\n";

		string vsStructOut = "struct VertexOut\n{ \n  float4 sv_positon : SV_POSITION;\n";
		int colorCount = 0, posCount = 0, normalCount = 0, texCount = 0, otherCount = 0;
		for (auto iter = interParams.begin(); iter != interParams.end(); iter++) {
			string name = iter->first;
			if (name == "sv_position") continue;
			string type = iter->second;
			string namelower, semantic;
			namelower.resize(16);
			//semantic label
			transform(name.begin(), name.end(), namelower.begin(), ::tolower);
			if (name.find("pos") != string::npos) semantic = "POSITION" + posCount++;
			else if (name.find("normal") != string::npos) semantic = "NORMAL" + normalCount++;
			else if (name.find("color") != string::npos) semantic = "COLOR" + colorCount++;
			else if (name.find("tex") != string::npos) semantic = "TEXCORD" + texCount++;
			else semantic = "UNKNOWN" + otherCount++;

			vsStructOut += "  " + type + " " + name + " : " + semantic + ";\n";
		}
		vsStructOut += "}\n\n";

		string vsDef = "VertexOut Vs(VertexIn vs_in)"
		"{\n  VertexOut vs_out=(VertextOut)0.0f\n";

		for (const auto& str : vsCodeLines) 
			vsDef += (string("  ")+str+"\n");
		vsDef += "  return vs_out;\n}";

		vsCodeLines.clear();
		vsOutput = vsBufferStrut+vsStructIn + vsStructOut + vsDef;

		//generate ps
		string psResource = GeneratePSShaderResourceCode();
		string psDef = "float4 Ps(VertexOut ps_in) : SV_Target\n{\n";
		for (const auto& str : vsCodeLines)
			vsDef += (string("  ") + str + "\n");
		psDef += string("  return ") + psReturnObj.GetByName("name").GetString() + ";\n}";
		psOutput = psResource + "\n" + psDef;

	}
	else {
		return false; //error
	}

	return true;
}
#pragma endregion

#pragma region GetCompliedInfo
ShaderResourceMeta ShaderComplier::GetResourceMeta()
{
	return resourceMeta;
}

bool ShaderComplier::HasDefaultRes(void** buff)
{
	if (hasDefaultRes) {
		*buff = buffer;
	}
	return hasDefaultRes;
}

std::vector<ShaderTexture> ShaderComplier::GetShaderTextures()
{
	return shaderTextures;
}

ShaderState ShaderComplier::GetShaderState()
{
	return shaderState;
}

#pragma endregion

#pragma region Configure Shader Resource
void ShaderComplier::CreateSampleState(const char* sampleStateName, int _filter, int _u, int _v)
{
	Shader::Filter filter;
	Shader::Address u, v;
	filter = static_cast<Shader::Filter>(_filter);
	u = static_cast<Shader::Address>(_u);
	v = static_cast<Shader::Address>(_v);

	SampleState sampleState(sampleStateName, filter, u, v);
	sampleStates.insert(sampleStateName);

	//Send sampleState and its Name into Render System;
	shaderState.sampleStates.push_back(sampleState);
}

void ShaderComplier::AddTexture(const char* textureName)
{
	textures.insert(textureName);
	resourceMeta.AddItem(ShaderResourceItem(textureName, ShaderResourceType::Texture));
}

void ShaderComplier::AddTextureA(const char* textureName, const char* textureDir) {
	textures.insert(textureName);
	resourceMeta.AddItem(ShaderResourceItem(textureName, ShaderResourceType::Texture));
	shaderTextures.push_back(ShaderTexture(textureName, textureDir, TEXTURE_TYPE::NONE));
}

void ShaderComplier::SetRenderQueue(int _queue)
{
	Shader::RenderQueue queue;
	queue = static_cast<Shader::RenderQueue>(_queue);
	shaderState.renderQueue = queue;
}

void ShaderComplier::AddFloat(const char* name)
{
	resourceMeta.AddItem(ShaderResourceItem(name, ShaderResourceType::FLOAT));
}

void ShaderComplier::AddFloat2(const char* name)
{
	resourceMeta.AddItem(ShaderResourceItem(name, ShaderResourceType::FLOAT2));
}

void ShaderComplier::AddFloat3(const char* name)
{
	resourceMeta.AddItem(ShaderResourceItem(name, ShaderResourceType::FLOAT3));
}

void ShaderComplier::AddFloat4(const char* name)
{
	resourceMeta.AddItem(ShaderResourceItem(name, ShaderResourceType::FLOAT4));
}

void ShaderComplier::AddMat3x3(const char* name)
{
	resourceMeta.AddItem(ShaderResourceItem(name, ShaderResourceType::MAT3x3));
}

void ShaderComplier::AddMat4x4(const char* name)
{
	resourceMeta.AddItem(ShaderResourceItem(name, ShaderResourceType::MAT4x4));
}

void ShaderComplier::UseAo(bool useAo)
{
	shaderState.useAo = useAo;
}

void ShaderComplier::UseShadow(bool useShadow)
{
	shaderState.useShadow = useShadow;
}

void ShaderComplier::AddFloatA(const char* name, float x)
{

}

void ShaderComplier::AddFloat2A(const char* name, float x, float y)
{

}

void ShaderComplier::AddFloat3A(const char* name, float x, float y, float z)
{

}

void ShaderComplier::AddFloat4A(const char* name, float x, float y, float z, float w)
{

}


#pragma endregion

#pragma region GlobalEnv Helpers
LObject ShaderComplier::GetVal(LObject rTable)
{
	const char* rName = rTable.GetByName("name").GetString();
	LObject table;
	table.AssignNewTable(scriptEnv.GetState());
	char temp[16];
	sprintf_s(temp, "_Id%d", GetRandomNewId());
	table.SetString("name", temp);
	table.SetString("raw", rName);
	string type = rTable.GetByName("type").GetString();
	table.SetString("type", type.c_str());
	LObject getValueMetatable = rTable.GetMetatable();
	if (getValueMetatable.IsNil())
		getValueMetatable = scriptEnv.GetState()->GetGlobals().GetByName("getvalue_metatable");
	table.SetMetatable(getValueMetatable);
	if (mode == Mode::vs) {
		string statment = type + " " + string(temp) + "=" + rName + ";";
		vsCodeLines.push_back(statment);
	}
	else if (mode == Mode::ps) {
		string statment = type + " " + string(temp) + "=" + rName + ";";
		psCodeLines.push_back(statment);
	}
	return table;
}

LObject ShaderComplier::GetMatrix(const char* key)
{
	LObject table;
	table.AssignNewTable(scriptEnv.GetState());
	string rName = string("matrix.") + key;
	table.SetString("name", rName.c_str());
	table.SetString("type", "float4x4");
	LObject getValueMetatable = scriptEnv.GetState()->GetGlobals().GetByName("getvalue_metatable");
	table.SetMetatable(getValueMetatable);
	return table;
}

LObject ShaderComplier::GetVsIn(const char* key)
{
	LObject table;
	table.AssignNewTable(scriptEnv.GetState());
	string rName = string("vs_in.") + key;
	table.SetString("name", rName.c_str());
	table.SetString("type", "float3");
	LObject getValueMetatable = scriptEnv.GetState()->GetGlobals().GetByName("getvalue_metatable");
	table.SetMetatable(getValueMetatable);
	return table;
}

void ShaderComplier::SetVsOut(const char* key, LObject val)
{
	interParams.insert(std::make_pair(key, val.GetByName("type").GetString()));
	vsCodeLines.pop_back();
	string stat = string("vs_out.") + key + "=";
	string rName = val.GetByName("raw").GetString();
	stat += rName + ";";
	vsCodeLines.push_back(stat);
}

LObject ShaderComplier::GetPsIn(const char* key)
{
	LObject table;
	if (interParams.find(key) == interParams.end())
		return table; //error
	table.AssignNewTable(scriptEnv.GetState());
	string rName = string("ps_in.") + key;
	table.SetString("name", rName.c_str());
	table.SetString("type", interParams.find(key)->second.c_str());
	LObject getValueMetatable = scriptEnv.GetState()->GetGlobals().GetByName("getvalue_metatable");
	table.SetMetatable(getValueMetatable);
	return table;
}

#pragma endregion

#pragma region Reload Shader Type
LObject ShaderComplier::GetFloat(LObject table)
{
	return LObject();
}

LObject ShaderComplier::GetFloat2(LObject x, LObject y)
{
	return LObject();
}

LObject ShaderComplier::GetFloat3(LObject x, LObject y, LObject z)
{
	return LObject();
}

LObject ShaderComplier::GetFloat4(LObject x, LObject y, LObject z, LObject w)
{
	LObject table;
	table.AssignNewTable(scriptEnv.GetState());
	table.SetMetatable(scriptEnv.GetState()->GetGlobals().GetByName("float4metatable"));
	if (y.IsNil()) {
		//float4(float4)
		//unsupported 
	}
	else if (z.IsNil()) {
		if (x.IsTable() && !strcmp(x.GetByName("type").GetString(), "float3")) {
			//float4(float3,float)
			string yName;
			if (y.IsNumber()) {
				char temp[16];
				sprintf(temp, "%.3f", y.GetNumber());
				yName = temp;
			}
			else {
				yName = y.GetByName("name").GetString();
			}
			string rName = string("float4(") + x.GetByName("name").GetString() + "," + yName + ")";
			table.SetString("name", rName.c_str());
			table.SetString("type", "float4");
		}
		else {
			//float4(float,float3)
		}
	}
	else if (!w.IsNil()) {
		//float4(float,float,float,float)
	}
	return table;
}
#pragma endregion

#pragma region Reload Shader Opeators

//Not check type yet
LObject ShaderComplier::OperatorReload(LObject& l1, LObject& l2, const char* operators, const char* metaTable) {
	LObject table;
	table.AssignNewTable(scriptEnv.GetState());
	table.SetMetatable(scriptEnv.GetState()->GetGlobals().GetByName(metaTable));
	table.SetString("type", l1.GetByName("type").GetString());

	string lName, rName;

	if (l1.IsTable() && l2.IsTable()) {
		lName = l1.GetByName("name").GetString();
		rName = l2.GetByName("name").GetString();
	}
	else if (l1.IsTable()) {
		lName = l1.GetByName("name").GetString();
		char temp[16]; sprintf_s(temp, "%.3f", l2.GetNumber());
		rName = temp;
	}
	else if (l2.IsTable()) {
		rName = l2.GetByName("name").GetString();
		char temp[16]; sprintf_s(temp, "%.3f", l1.GetNumber());
		lName = temp;
	}
	table.SetString("name", (string("(") + lName + operators + rName + ")").c_str());
	return table;
}

LObject ShaderComplier::floatmul(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "*", "floatmetatable");
}

LObject ShaderComplier::floatadd(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "+", "floatmetatable");
}

LObject ShaderComplier::floatsub(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "-", "floatmetatable");
}

LObject ShaderComplier::floatdiv(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "-", "floatmetatable");
}

LObject ShaderComplier::float2mul(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "*", "floatmetatable");
}

LObject ShaderComplier::float2add(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "+", "float2metatable");
}

LObject ShaderComplier::float2sub(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "-", "float2metatable");
}

LObject ShaderComplier::float2div(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "-", "float2metatable");
}

LObject ShaderComplier::float3mul(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "*", "float2metatable");
}

LObject ShaderComplier::float3add(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "+", "float2metatable");
}

LObject ShaderComplier::float3sub(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "-", "float3metatable");
}

LObject ShaderComplier::float3div(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "-", "float3metatable");
}

LObject ShaderComplier::float4mul(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "*", "float4metatable");
}

LObject ShaderComplier::float4add(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "+", "float4metatable");
}

LObject ShaderComplier::float4sub(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "-", "float4metatable");
}

LObject ShaderComplier::float4div(LObject l1, LObject l2)
{
	return OperatorReload(l1, l2, "-", "float4metatable");
}

#pragma endregion

#pragma region Reload Shader Functions
LObject ShaderComplier::GetMul(LObject l1, LObject l2)
{
	LObject table;
	table.AssignNewTable(scriptEnv.GetState());
	string l1Type = l1.GetByName("type").GetString();
	table.SetMetatable(scriptEnv.GetState()->GetGlobals().GetByName((l1Type + "metatable").c_str()));
	table.SetString("type", l1Type.c_str());
	table.SetString("name", (string("mul(") + l1.GetByName("name").GetString() + "," + l2.GetByName("name").GetString() + ")").c_str());
	return table;
}

LObject ShaderComplier::GetDot(LObject l1, LObject l2) {
	return LObject();
}

LObject ShaderComplier::Sample(const char* textureName, LObject uv, const char* sampleState) {
	LObject table;
	table.AssignNewTable(scriptEnv.GetState());
	if (!uv.IsTable())  return table; //error
	if (!strcmp(uv.GetByName("type").GetString(), "float2")) return table; //error
	if (sampleStates.find(sampleState) == sampleStates.end() || textures.find(textureName) == textures.end()) return table; //error
	table.SetMetatable(scriptEnv.GetState()->GetGlobals().GetByName("float4metatable"));
	table.SetString("type", "float4");
	table.SetString("name", (string(textureName)
		+ ".Sample(" + sampleState + "," + uv.GetByName("name").GetString() + ")").c_str());
	return table;
}

LObject ShaderComplier::Normalize(LObject vec) {
	LObject table;
	if (!vec.IsTable()) return table; //error
	table.AssignNewTable(scriptEnv.GetState());
	table.SetString("type", vec.GetByName("type").GetString());
	table.SetString("name", (string("normalize(") + vec.GetByName("name").GetString() + ")").c_str());
	string vecType = vec.GetByName("type").GetString();
	table.SetMetatable(scriptEnv.GetState()->GetGlobals().GetByName((vecType + "metatable").c_str()));
	return table;
}

#pragma endregion

#pragma region ShaderResourceConfig
ShaderResourceConfig::ShaderResourceConfig(std::string _fileName, ShaderResourceMeta _meta)
	:configFileName(_fileName), meta(_meta), buffer(nullptr)
{
	InitEnv();
	LoadAndParse();
}

ShaderResourceConfig::~ShaderResourceConfig() {}

void* ShaderResourceConfig::GetBuff()
{
	return this->buffer;
}

std::vector<ShaderTexture> ShaderResourceConfig::GetTextures()
{
	return this->textures;
}

void ShaderResourceConfig::InitEnv()
{
	scriptEnv.GetState()->GetGlobals().RegisterDirect("float2", *this, &ShaderResourceConfig::Float2);
	scriptEnv.GetState()->GetGlobals().RegisterDirect("float3", *this, &ShaderResourceConfig::Float3);
	scriptEnv.GetState()->GetGlobals().RegisterDirect("float4", *this, &ShaderResourceConfig::Float4);
}

bool ShaderResourceConfig::LoadAndParse()
{
	void* buffer = meta.BuildBuff();

	scriptEnv.LoadAndExecuteScriptResource(configFileName.c_str());
	LuaPlus::LuaObject config = scriptEnv.GetState()->GetGlobals().GetByName("config");
	if (config.IsNil()) {
		config = scriptEnv.GetState()->GetGlobals().GetByName("data");
	}
	if (config.IsNil() || !config.IsTable()) {
		GCC_ERROR("invaild material instance!");
		return false;
	}

	std::map<std::string, ShaderResourceType> resourceItemNames;
	const auto metaItems = meta.GetItems();
	for (auto iter = metaItems.begin(); iter != metaItems.end(); iter++) {
		resourceItemNames.insert(make_pair(iter->name, iter->type));
	}

	for (LuaPlus::LuaTableIterator it(config); it; it.Next()) {
		const char* key = it.GetKey().GetString();
		LuaPlus::LuaObject value = it.GetValue();
		auto iter = resourceItemNames.find(key);
		if (iter == resourceItemNames.end()) continue;

		switch (iter->second) {
		case ShaderResourceType::Texture: {
			textures.push_back(ShaderTexture(key, value.GetString()));
		}break;
		case ShaderResourceType::FLOAT: {
			float val = value.GetFloat();
			meta.SetVal(buffer, key, val);
		}break;
		case ShaderResourceType::FLOAT2: {
			if (value.IsFunction()) {
				LuaPlus::LuaFunctionVoid func(value);
				func();
				XMFLOAT2 float2;
				float2.x = temp.x;
				float2.y = temp.y;
				meta.SetVal(buffer, key, float2);
			}
		}break;
		case ShaderResourceType::FLOAT3: {
			if (value.IsFunction()) {
				LuaPlus::LuaFunctionVoid func(value);
				func();
				XMFLOAT3 float3;
				float3.x = temp.x;
				float3.y = temp.y;
				float3.z = temp.z;
				meta.SetVal(buffer, key, float3);
			}
		}break;
		case ShaderResourceType::FLOAT4: {
			if (value.IsFunction()) {
				LuaPlus::LuaFunctionVoid func(value);
				func();
				meta.SetVal(buffer, key, temp);
			}
		}break;
		case ShaderResourceType::MAT3x3: {

		}break;
		case ShaderResourceType::MAT4x4: {

		}break;
		default:break;
		}
	}

}

void ShaderResourceConfig::Float2(float x, float y)
{
	temp.x = x;
	temp.y = y;
}

void ShaderResourceConfig::Float3(float x, float y, float z)
{
	temp.x = x;
	temp.y = y;
	temp.z = z;
}

void ShaderResourceConfig::Float4(float x, float y, float z, float w)
{
	temp.x = x;
	temp.y = y;
	temp.z = z;
	temp.w = w;
}
#pragma endregion