#pragma once
#include"../Engine/Engine.h"

struct PatchInfo {
	const int cellsPerPatch = 64;
	unsigned int numPatchVertices;
	unsigned int numPatchQuadFaces;
	unsigned int numPatchVertRows;
	unsigned int numPatchVertCols;
	std::vector<XMFLOAT2> patchBoundsY;
};

class TerrainHeightMap;
class TerrainShader {
public:
	TerrainShader() = delete;
	TerrainShader(shared_ptr<TerrainHeightMap> _heightMap, PatchInfo _info);

	bool ComplieShader();
	bool CreateVertexLayout();
	bool BuildQuadPatchVB();
	bool BuildQuadPatchIB();
	bool BuildHeightmapSRV();

private:
	PatchInfo patchInfo;
	shared_ptr<TerrainHeightMap> heightMap;

	static const int CellsPerPatch = 64;

	ID3D11InputLayout* vertexLayout11;
	   
	ID3D11VertexShader* vertexShader;
	ID3D11HullShader* hullShader;
	ID3D11DomainShader* domainShader;
	ID3D11PixelShader* pixelShader;

	ID3D11Buffer* quadPatchVB;
	ID3D11Buffer* quadPatchIB;

	ID3D11ShaderResourceView* heightMapSRV;
	ID3D11ShaderResourceView* layerMapArraySRV;
	ID3D11ShaderResourceView* blendMapSRV;
};