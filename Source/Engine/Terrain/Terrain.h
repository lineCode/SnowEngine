#pragma once
#include"../Engine/Engine.h"
#include"TerrainShader.h"
#include<string>
#include<vector>
#include<memory>

class Terrain {
public:
	Terrain() = default;
private:
	std::string heightMapFileName;

	unsigned int heightMapWidth;
	unsigned int heightMapHeight;
	float heightScale; 
	float cellSpacing;

	PatchInfo patchInfo;
	shared_ptr<TerrainHeightMap> heightMap;
	shared_ptr<TerrainShader> terrainShader;
};

class TerrainHeightMap {
	friend class TerrainShader;

public:
	TerrainHeightMap() = delete;
	TerrainHeightMap(unsigned int _heightMapWidth, unsigned int _heightMapHeight, float _heightScale = 50.0f, float _cellSpacing = 0.5f);

	void LoadHeightMap(std::string heightMapFileName);
	void Smooth();

	float GetWidth() const;
	float GetDepth() const;
	float GetHeight(float x, float z) const;

private:
	std::string heightMapName;
	std::vector <float> heightMap;

	unsigned int heightMapWidth;
	unsigned int heightMapHeight;

	float heightScale;
	float cellSpacing;

	bool InBounds(int i, int j);
	float Average(int i, int j);
};

class HeightMapLoader: public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return true; }
	virtual bool VDiscardRawBufferAfterLoad() { return false; }
	virtual unsigned int VGetLoadedResourceSize(char* rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle) { return true; }
	virtual std::string VGetPattern() { return "*.raw"; }
};
