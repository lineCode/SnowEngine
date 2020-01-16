#include"../Msvc/EngineStd.h"
#include"Terrain.h"

TerrainHeightMap::TerrainHeightMap(unsigned int _heightMapWidth, unsigned int _heightMapHeight, float _heightScale, float _cellSpacing):
heightMapWidth(_heightMapWidth),heightMapHeight(_heightMapHeight),heightScale(_heightScale),cellSpacing(_cellSpacing)
{}

bool TerrainHeightMap::InBounds(int i, int j)
{
	// True if ij are valid indices; false otherwise.
	return (i >= 0 && i < (int)heightMapHeight) && (j >= 0 && j < (int)heightMapWidth);
}

void TerrainHeightMap::LoadHeightMap(std::string heightMapFileName)
{
	Resource resource(heightMapFileName);
	shared_ptr<ResHandle> resourceHandle = g_pApp->m_ResCache->GetHandle(&resource);
	heightMap.resize(heightMapHeight * heightMapWidth);

	for (UINT i = 0; i < heightMapHeight * heightMapWidth; i++)
	{
		heightMap[i] = (resourceHandle->Buffer[i] / 255.0f) * heightScale;
	}
}

void TerrainHeightMap::Smooth()
{
	if (heightMap.size() == 0) 
	{
		GCC_ERROR("should load heightMap first!");
		return;
	}

	std::vector<float> dest(heightMap.size());
	for (UINT i = 0; i < heightMapHeight; ++i)
	{
		for (UINT j = 0; j <heightMapWidth; ++j)
		{
			dest[i * heightMapWidth + j] = Average(i, j);
		}
	}
	heightMap = dest;
}

float TerrainHeightMap::Average(int i, int j)
{
	float avg = 0.0f;
	float num = 0.0f;

	for (int m = i - 1; m <= i + 1; ++m)
	{
		for (int n = j - 1; n <= j + 1; ++n)
		{
			if (InBounds(m, n))
			{
				avg += heightMap[m * heightMapWidth + n];
				num += 1.0f;
			}
		}
	}
	return avg / num;
}

float TerrainHeightMap::GetWidth() const
{
	return (heightMapWidth - 1) * cellSpacing;
}

float TerrainHeightMap::GetDepth() const
{
	return (heightMapHeight - 1) * cellSpacing;
}

float TerrainHeightMap::GetHeight(float x, float z)const
{
	// Transform from terrain local space to "cell" space.
	float c = (x + 0.5f * GetWidth()) / cellSpacing;
	float d = (z - 0.5f * GetDepth()) / -cellSpacing;

	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = heightMap[row * heightMapWidth + col];
	float B = heightMap[row * heightMapWidth + col + 1];
	float C = heightMap[(row + 1) * heightMapWidth + col];
	float D = heightMap[(row + 1) * heightMapWidth + col + 1];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if (s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s * uy + t * vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f - s) * uy + (1.0f - t) * vy;
	}
}

unsigned int HeightMapLoader::VGetLoadedResourceSize(char* rawBuffer, unsigned int rawSize)
{
	return rawSize;
}
