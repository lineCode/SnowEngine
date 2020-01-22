#pragma once
#include"../Engine/Engine.h"
#include"NScene.h"
#include"../Render/RenderResource.h"
#include "../Utilities/xnacollision.h"

#include<memory>
#include<string>
#include<vector>
#include<set>

using std::set;
using std::vector;
using std::shared_ptr;
using std::string;

using ActorId = int;

//Class NSceneNode 
//Defines scenenode baisc properities
class NSceneNode {
	friend class NScene;
public:
	NSceneNode();
	NSceneNode(int actorId);
	NSceneNode(int actorId,bool isStatic);
	NSceneNode(const string& name, int actorId = INVAILD_ACOTR_ID,bool isStatic = false);
	NSceneNode(const string& name, const Mat4x4&, int actorId = INVAILD_ACOTR_ID, bool isStatic = false);

	virtual bool VAddChild(shared_ptr<NSceneNode> kid);
	virtual bool VRemoveChild(ActorId id);

	Vec3 GetPosition() const;

	//if static. only valid when called on first time
	void SetPosition(const Vec3& pos);

	Vec3 GetDirection() const;

	//if static. only valid when called on first time
	void SetTransform(const Mat4x4* toWorld, const Mat4x4* fromWorld = NULL);

	bool IsStatic();

protected:
	//store parent to avoid multi-parent
	NSceneNode* parent;
	//store childrens to calculate position
	vector<shared_ptr<NSceneNode>> childrens;
	//store children's id to add/remove child quickly
	set<ActorId> childrenIds;

	ActorId actorId;
	string name;
	Mat4x4	toWorld, fromWorld;

	bool isStatic = false;
	bool hasSetPos = false;
};

class NSceneOctreeNode;

//Renderable Node
class NRenderableSceneNode :public NSceneNode {
public:
	NRenderableSceneNode() = delete;
	NRenderableSceneNode(int actorId = INVAILD_ACOTR_ID, bool isStatic = false);
	NRenderableSceneNode(const string& name,int actorId = INVAILD_ACOTR_ID, bool isStatic = false);
	NRenderableSceneNode(Render::RenderType,const string&, int actorId = INVAILD_ACOTR_ID, bool isStatic = false);

	void SetSphereRadius(float radius);
	float GetSphereRadius();
	void SetRenderPass(Render::RenderPass);
	Render::RenderPass GetRenderPass();
	Render::RenderType GetRenderType();
	void SetContainer(NSceneOctreeNode* _container);
	NSceneOctreeNode* GetContainer();

protected:
	float radius=0.0f;
	const Render::RenderType renderType;
	Render::RenderPass renderPass;
	bool visable;
	NSceneOctreeNode* container;
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

class NFrustum {
	friend class NCameraNode;
public:
	NFrustum();

	float GetFov();
	float GetRatio();
	float GetNearZ();
	float GetFarZ();

	void Init(float _fov,float _ratio,float _nearZ,float _farZ);
	
protected:

	float fov;
	float ratio;
	float rightSlope;           // Positive X slope (X/Z).
	float leftSlope;            // Negative X slope.
	float topSlope;             // Positive Y slope (Y/Z).
	float bottomSlope;          // Negative Y slope.
	float nearz,farz;            // Z of the near plane and far plane.
};

class NCameraNode :public NSceneNode {
public:
	NCameraNode();
	NCameraNode(float _fov, float _nearz, float _farz);

	void SetFov(float _fov);

	const XNA::Frustum GetDXFrustum();
	const NFrustum GetFrustum() const;
	const Mat4x4 GetView() const;
	const Mat4x4 GetProjection() const;
	const Mat4x4 GetViewProjection() const;

protected:
	void Init(float _fov, float _nearz, float _farz);

	NFrustum frustum;
	Mat4x4 projection;
};
