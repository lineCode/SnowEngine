#pragma once

#include"TerrainShader.h"
#include<memory>

class TerrainSceneNode {
public:
	TerrainSceneNode() = default;

private:
	std::unique_ptr<TerrainShader> shader;
};
