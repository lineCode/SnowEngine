#include"../Msvc/EngineStd.h"
#include"TerrainShader.h"
#include"Terrain.h"

const D3D11_INPUT_ELEMENT_DESC TerrainVertexLayout[] =
{
	{ "PosL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "Tex",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BoundsY",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

struct Terrain
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
	XMFLOAT2 BoundsY;
};

TerrainShader::TerrainShader(shared_ptr<TerrainHeightMap> _heightMap, PatchInfo _info)
	:heightMap(_heightMap), patchInfo(_info), vertexLayout11(nullptr), vertexShader(nullptr),
	hullShader(nullptr), domainShader(nullptr), pixelShader(nullptr), quadPatchVB(nullptr),
	quadPatchIB(nullptr), heightMapSRV(nullptr), layerMapArraySRV(nullptr), blendMapSRV(nullptr)
{}

TerrainShader::~TerrainShader()
{
	SAFE_RELEASE(vertexLayout11);
	SAFE_RELEASE(vertexShader);
	SAFE_RELEASE(hullShader);
	SAFE_RELEASE(domainShader);
	SAFE_RELEASE(pixelShader);
	SAFE_RELEASE(quadPatchIB);
	SAFE_RELEASE(quadPatchVB);
	SAFE_RELEASE(heightMapSRV);
	SAFE_RELEASE(layerMapArraySRV);
	SAFE_RELEASE(blendMapSRV);
}

bool TerrainShader::ComplieShader()
{
	Resource resource("Effects//Terrain.hlsl");
	auto handle= g_pApp->m_ResCache->GetHandle(&resource);

}

bool TerrainShader::ComplieVsShader()
{

}

bool TerrainShader::ComplieHsShader()
{

}

bool TerrainShader::ComplieDsShader()
{

}

bool TerrainShader::CompliePsShader()
{

}

bool TerrainShader::CreateVertexLayoutAndBuffer()
{

}

bool TerrainShader::BuildHeightmapSRV()
{
	ID3D11Device* device = DXUTGetD3D11Device();
	D3D11_TEXTURE2D_DESC texDesc;

	texDesc.Width = heightMap->heightMapWidth;
	texDesc.Height = heightMap->heightMapHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	std::vector<HALF> hmap(heightMap->heightMap.size());
	std::transform(heightMap->heightMap.begin(), heightMap->heightMap.end(),
		hmap.begin(), XMConvertFloatToHalf);
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &hmap[0];
	data.SysMemPitch = heightMap->heightMapWidth * sizeof(HALF);
	data.SysMemSlicePitch = 0;
	ID3D11Texture2D* hmapTex = 0;
	if (FAILED(device->CreateTexture2D(&texDesc, &data, &hmapTex))) {
		GCC_ERROR("create heightMap Texture Resource Failed!");
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	if (FAILED(device->CreateShaderResourceView(hmapTex, &srvDesc, &heightMapSRV))) {
		GCC_ERROR("create heightMap SRV Failed!");
		SAFE_RELEASE(hmapTex);
		return false;
	}

	SAFE_RELEASE(hmapTex);
	return true;
}

bool TerrainShader::BuildQuadPatchVB()
{
	ID3D11Device* device = DXUTGetD3D11Device();
	std::vector<Terrain> patchVertices(patchInfo.numPatchVertRows * patchInfo.numPatchVertCols);

	float halfWidth = 0.5f * heightMap->GetWidth();
	float halfDepth = 0.5f * heightMap->GetDepth();

	float patchWidth = heightMap->GetWidth() / (patchInfo.numPatchVertCols - 1);
	float patchDepth = heightMap->GetDepth() / (patchInfo.numPatchVertRows - 1);
	float du = 1.0f / (patchInfo.numPatchVertCols - 1);
	float dv = 1.0f / (patchInfo.numPatchVertRows - 1);

	for (UINT i = 0; i < patchInfo.numPatchVertRows; ++i)
	{
		float z = halfDepth - i * patchDepth;
		for (UINT j = 0; j < patchInfo.numPatchVertCols; ++j)
		{
			float x = -halfWidth + j * patchWidth;

			patchVertices[i * patchInfo.numPatchVertCols + j].Pos = XMFLOAT3(x, 0.0f, z);

			// Stretch texture over grid.
			patchVertices[i * patchInfo.numPatchVertCols + j].Tex.x = j * du;
			patchVertices[i * patchInfo.numPatchVertCols + j].Tex.y = i * dv;
		}
	}

	// Store axis-aligned bounding box y-bounds in upper-left patch corner.
	for (UINT i = 0; i < patchInfo.numPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < patchInfo.numPatchVertCols - 1; ++j)
		{
			UINT patchID = i * (patchInfo.numPatchVertCols - 1) + j;
			patchVertices[i * patchInfo.numPatchVertCols + j].BoundsY = patchInfo.patchBoundsY[patchID];
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Terrain) * patchVertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &patchVertices[0];
	if (FAILED(device->CreateBuffer(&vbd, &vinitData, &quadPatchVB))) {
		GCC_ERROR("falied to create terrain index buffer!");
		return false;
	}
	return true;
}

bool TerrainShader::BuildQuadPatchIB()
{
	ID3D11Device* device = DXUTGetD3D11Device();

	std::vector<USHORT> indices(patchInfo.numPatchQuadFaces * 4); // 4 indices per quad face

	// Iterate over each quad and compute indices.
	int k = 0;
	for (UINT i = 0; i < patchInfo.numPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < patchInfo.numPatchVertCols - 1; ++j)
		{
			// Top row of 2x2 quad patch
			indices[k] = i * patchInfo.numPatchVertCols + j;
			indices[k + 1] = i * patchInfo.numPatchVertCols + j + 1;

			// Bottom row of 2x2 quad patch
			indices[k + 2] = (i + 1) * patchInfo.numPatchVertCols + j;
			indices[k + 3] = (i + 1) * patchInfo.numPatchVertCols + j + 1;

			k += 4; // next quad
		}
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(USHORT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	if (FAILED(device->CreateBuffer(&ibd, &iinitData, &quadPatchIB))) {
		GCC_ERROR("falied to create terrain index buffer!");
		return false;
	}
	return true;
}
