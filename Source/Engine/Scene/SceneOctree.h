#pragma once
#include<xnamath.h>
#include "../Utilities/xnacollision.h"
#include<vector>
#include"SceneNodes.h"

using UINT = unsigned int;

struct SceneOctreeNode;
class Scene;

class SceneOctree
{
public:
	SceneOctree();
	SceneOctree(Scene* _scene):SceneOctree() { scene = _scene; }
	~SceneOctree();

	//Build Octree Call when init Scene System
	void Build(const std::vector<shared_ptr<ISceneNode>>& _allSceneNodes);
	//Ray Trace Boost
	bool RayOctreeIntersect(FXMVECTOR rayPos, FXMVECTOR rayDir);
	//Frustum View Culling
	std::vector<shared_ptr<ISceneNode>> FrustumCulling(CameraNode* cameraNode);
	//Move Node to New Position
	void MoveNode(ISceneNode* node, Vec3 newPostion);
	//Add Node Should be called  after Scene has been almostly built
	void AddSceneNode(shared_ptr<ISceneNode> node);

private:
	XNA::AxisAlignedBox BuildAABB();
	void BuildOctree(SceneOctreeNode* parent,const std::vector<shared_ptr<ISceneNode>>& nodes, int level);
	bool RayOctreeIntersect(SceneOctreeNode* parent, FXMVECTOR rayPos, FXMVECTOR rayDir);
	SceneOctreeNode* GetNodeToPlaceSphere(XNA::Sphere& sphere);

private:
	SceneOctreeNode* mRoot;
	Scene* scene;
	std::vector<shared_ptr<ISceneNode>> allSceneNodes;
};

struct SceneOctreeNode
{
	XNA::AxisAlignedBox Bounds;

	//if a node can't be contained into a treenode's children
	//it will be placed here
	std::vector<shared_ptr<ISceneNode>> sceneNodes;

	SceneOctreeNode* Children[8];

	bool IsLeaf;

	SceneOctreeNode()
	{
		for (int i = 0; i < 8; ++i)
			Children[i] = 0;

		Bounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
		Bounds.Extents = XMFLOAT3(0.0f, 0.0f, 0.0f);

		IsLeaf = false;
	}

	~SceneOctreeNode()
	{
		for (int i = 0; i < 8; ++i)
			if (Children[i]) {
				delete Children[i];
			}
	}

	void Subdivide(XNA::AxisAlignedBox box[8])
	{
		XMFLOAT3 halfExtent(
			0.5f * Bounds.Extents.x,
			0.5f * Bounds.Extents.y,
			0.5f * Bounds.Extents.z);

		// "Top" four quadrants.
		box[0].Center = XMFLOAT3(
			Bounds.Center.x + halfExtent.x,
			Bounds.Center.y + halfExtent.y,
			Bounds.Center.z + halfExtent.z);
		box[0].Extents = halfExtent;

		box[1].Center = XMFLOAT3(
			Bounds.Center.x - halfExtent.x,
			Bounds.Center.y + halfExtent.y,
			Bounds.Center.z + halfExtent.z);
		box[1].Extents = halfExtent;

		box[2].Center = XMFLOAT3(
			Bounds.Center.x - halfExtent.x,
			Bounds.Center.y + halfExtent.y,
			Bounds.Center.z - halfExtent.z);
		box[2].Extents = halfExtent;

		box[3].Center = XMFLOAT3(
			Bounds.Center.x + halfExtent.x,
			Bounds.Center.y + halfExtent.y,
			Bounds.Center.z - halfExtent.z);
		box[3].Extents = halfExtent;

		// "Bottom" four quadrants.
		box[4].Center = XMFLOAT3(
			Bounds.Center.x + halfExtent.x,
			Bounds.Center.y - halfExtent.y,
			Bounds.Center.z + halfExtent.z);
		box[4].Extents = halfExtent;

		box[5].Center = XMFLOAT3(
			Bounds.Center.x - halfExtent.x,
			Bounds.Center.y - halfExtent.y,
			Bounds.Center.z + halfExtent.z);
		box[5].Extents = halfExtent;

		box[6].Center = XMFLOAT3(
			Bounds.Center.x - halfExtent.x,
			Bounds.Center.y - halfExtent.y,
			Bounds.Center.z - halfExtent.z);
		box[6].Extents = halfExtent;

		box[7].Center = XMFLOAT3(
			Bounds.Center.x + halfExtent.x,
			Bounds.Center.y - halfExtent.y,
			Bounds.Center.z - halfExtent.z);
		box[7].Extents = halfExtent;
	}
};