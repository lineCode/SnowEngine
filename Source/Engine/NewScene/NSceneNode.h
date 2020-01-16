#pragma once
#include"../Engine/Engine.h"
#include"../Render/RenderResource.h"

#include<memory>
#include<string>
#include<vector>
using std::vector;
using std::shared_ptr;

class NSceneNode {
public:
	virtual bool VAddChild(shared_ptr<NSceneNode> kid);
	virtual bool VRemoveChild(ActorId id);

	Vec3 GetPosition() const;
	void SetPosition(const Vec3& pos);
	Vec3 GetDirection() const;
	void SetTransform(const Mat4x4* toWorld, const Mat4x4* fromWorld = NULL);

protected:
	NSceneNode* parent;
	vector<shared_ptr<NSceneNode>> childrens;

	ActorId actorId;
	std::string name;
	Mat4x4	toWorld, fromWorld;
};

//Renderable Node
class NRenderSceneNode :NSceneNode {
public:

protected:
	float radius;
	Render::RenderPass renderPass;
	bool visable;
	SceneOctreeNode* container;
};

struct NAplhaSceneNode {
public:
	NAplhaSceneNode(std::shared_ptr<NSceneNode> _node, float _z) 
		:node(_node), z(_z) {}

	std::shared_ptr<NSceneNode> node;
	float z;
};

bool operator <(NAplhaSceneNode& n1, NAplhaSceneNode& n2)
{
	return n1.z < n2.z;
}
