#include "../Msvc/EngineStd.h"
#include"ShaderResource.h"

void ShaderResourceMeta::AddItem(const ShaderResourceItem& item)
{
	if (structSize < 0) items.push_back(item);
}

unsigned int ShaderResourceMeta::GetStructSize() 
{
	if (structSize > 0) return structSize;

	ReArrange();

	unsigned int size = 0;
	unsigned int float2Count = 0;
	unsigned int floatCount = 0;

	for (auto iter = items.begin(); iter != items.end(); iter++) 
	{
		if (iter->type == ShaderResourceType::Texture) continue;
		else if (iter->type == ShaderResourceType::MAT4x4) {
			posMap.insert(std::make_pair(iter->name, size));
			size += (sizeof(float) * 4 * 4);
		}
		else if (iter->type == ShaderResourceType::MAT3x3) {
			posMap.insert(std::make_pair(iter->name, size));
			size += (sizeof(float) * 4 * 3);
		}
		else if (iter->type == ShaderResourceType::FLOAT4 || iter->type == ShaderResourceType::FLOAT3) {
			posMap.insert(std::make_pair(iter->name, size));
			size += (sizeof(float) * 4);
		}
		else if (iter->type == ShaderResourceType::FLOAT2) {
			float2Count++;
			posMap.insert(std::make_pair(iter->name, size));
			size += (sizeof(float) * 2);
		}
		else if (iter->type == ShaderResourceType::FLOAT) {
			floatCount++;
		}
	}

	if (float2Count % 2 == 1 && floatCount == 0) 
	{
		size += (sizeof(float) * 2);
		return size;
	}

	for (auto iter = items.begin(); iter != items.end(); iter++) 
	{
		if (iter->type == ShaderResourceType::FLOAT) 
		{
			posMap.insert(std::make_pair(iter->name, size));
			size += sizeof(float);
		}
	}

	int temp = 0;
	if (float2Count % 2 == 1)
		temp = 2;
	temp += floatCount;
	if (temp % 4) {
		int empty = 4 - (temp % 4);
		size += empty;
	}

	structSize = size;
	return size;
}

void* ShaderResourceMeta::BuildBuff() 
{
	unsigned int buffSize = GetStructSize();
	void* buff = malloc(buffSize);
	memset(buff, 0, buffSize);
	if (!buff) return nullptr;
	return buff;
}