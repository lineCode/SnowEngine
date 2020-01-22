#pragma once
#include"NSceneNode.h"
#include"../Render/SceneRender.h"

#include<vector>
#include<map>
#include<memory>

using std::vector;
using std::map;
using std::shared_ptr;

class SceneRender;

class NScene final{
public:
	NScene();
	~NScene();

	void SetCamera(shared_ptr<NCameraNode> _camera);
	const shared_ptr<NCameraNode> GetCamera() const;

	bool AddSceneNode(ActorId id, shared_ptr<NSceneNode> node);
	bool AddSceneNode(shared_ptr<NSceneNode> node);

	HRESULT OnRender() ;

protected:
	map<ActorId, shared_ptr<NSceneNode> > actorMap;
	vector<shared_ptr<NSceneNode>> allSceneNodes;

	shared_ptr<NCameraNode> camera;
	
	SceneRender* sceneRender;

private:
	int GetRandomNewId();
};
