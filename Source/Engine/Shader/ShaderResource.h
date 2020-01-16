#pragma once

namespace Shader {
	enum class Filter {
		MIN_LINEAR_MAG_MIP_POINT = 34984553,
		MIN_MAG_MIP_LINEAR = 34984554,
		MIN_POINT_MAG_LINEAR_MIP_POINT = 34984555,
		ANISOTROPIC = 34984556
	};

	enum class Address {
		WRAP = 47867821,
		BORDER = 47867822,
		CLAMP = 47867823,
		MIRROR = 47867824
	};

	enum class RenderQueue {
		BackGround = 1000,
		Opaque = 2000,
		AlphaTest = 2450,
		Overlay = 4000,
		GeometryLast = 9000
	};
};

struct SampleState {
	std::string sampleName;
	Shader::Filter filter;
	Shader::Address addressu;
	Shader::Address addressv;
	SampleState() :filter(Shader::Filter::MIN_MAG_MIP_LINEAR), addressu(Shader::Address::WRAP), addressv(Shader::Address::WRAP) { }
	SampleState(const std::string _sampleName, Shader::Filter _filter, Shader::Address _u, Shader::Address _v) :
		sampleName(_sampleName), filter(_filter), addressu(_u), addressv(_v) {}
};

enum class TEXTURE_TYPE {
	ALBEDO,
	DIFFUSE,
	SPECULAR,
	NORMAL,
	MTTALLIC,
	ROUGHNESS,
	HEIGHT,
	AO,
	REFLECTION,
	LIGHTMAP,
	SHININESS,
	NONE
};

//can implement in a more elegant way
extern const char* TEXTTURE_TYPE_STR[] = {
	"ALBEDO",
	"DIFFUSE",
	"SPECULAR",
	"NORMAL",
	"MTTALLIC",
	"ROUGHNESS",
	"HEIGHT",
	"AO",
	"REFLECTION",
	"LIGHTMAP",
	"SHININESS",
	"NONE"
};

struct ShaderTexture
{
	ShaderTexture() = delete;
	ShaderTexture(const std::string& _name, const std::string& _path, TEXTURE_TYPE _type)
		:name(_name), path(_path), type(_type) {}
	ShaderTexture(const std::string& _name, const std::string& _path)
		:name(_name), path(_path), type(TEXTURE_TYPE::NONE) {}
	std::string name;
	std::string path;
	TEXTURE_TYPE type;
};

enum class ShaderResourceType {
	Texture = 0,
	FLOAT = 1,
	FLOAT2 = 2,
	FLOAT3 = 3,
	FLOAT4 = 4,
	MAT3x3 = 5,
	MAT4x4 = 6,
};

struct ShaderResourceItem {
	ShaderResourceItem() = delete;
	ShaderResourceItem(std::string _name, ShaderResourceType _type) :name(_name), type(_type) {}
	ShaderResourceType type;
	std::string name;
};

bool operator < (ShaderResourceItem& l1, ShaderResourceItem& l2) {
	return l1.type < l2.type;
}

struct ShaderResourceMeta {
	ShaderResourceMeta() = default;

	const std::vector<ShaderResourceItem>& GetItems() { return items; }
	void AddItem(const ShaderResourceItem& item);

private:
	std::map<std::string, unsigned int> posMap;

	void ReArrange() {
		sort(items.begin(), items.end(), [](auto& l1, auto& l2) {return l2 < l1; });
	}

	std::vector<ShaderResourceItem> items;
	int structSize = -1;

public:
	//Pack The HLSL CBubffer Struct. The Struct Contains Data Defined By User
	//If Called This Method. You won't be able to modify this struct any more
	unsigned int GetStructSize();

	void* BuildBuff();

	template<class T>
	void SetVal(void* buff, const std::string& name, T val);
};

template<class T>
void ShaderResourceMeta::SetVal(void* buff, const std::string& name, T val) {
	auto iter = posMap.find(name);
	if (iter == posMap.end()) return;
	auto pos = iter->second;
	memcpy((char*)buff + pos, &val, sizeof(val));
}

struct ShaderState {
	bool useAo = false;
	bool useShadow = false;
	Shader::RenderQueue renderQueue;
	std::vector<SampleState> sampleStates;
};

const D3D11_INPUT_ELEMENT_DESC D3D11VertexLayout[] =
{
	{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

struct cbMatrices
{
	Mat4x4 worldviewproj;
	Mat4x4 invworldviewproj;
	Mat4x4 world;
	Mat4x4 view;
	Mat4x4 proj;
	Mat4x4 invworld;
	Mat4x4 invview;
	Mat4x4 invproj;
	XMFLOAT4 view_position;
	XMFLOAT4 position;
	float nearz;
	float farz;
	float fov;
	float time;
};
