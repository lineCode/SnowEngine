#pragma once
#include"../Engine/Engine.h"
#include"../NewScene/NScene.h"

#include"MeshRender.h"
#include"ParticleRender.h"
#include"TerrainRender.h"

//ScenerRender
//One Scene contains One SceneRender

class SceneRender {
public:
	SceneRender()=delete;
	SceneRender(NScene* _scene) :scene(_scene) {}

	HRESULT Render();

private:
	NScene* scene;

	MeshRender meshRender;
	ParticleRender particleRender;
	TerrainRender terrainRender;

};