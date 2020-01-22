#pragma once

#include<xnamath.h>
#include "../Utilities/xnacollision.h"
#include<vector>
#include"NScene.h"

using std::vector;
using std::string;
using std::map;
using std::shared_ptr;

struct NSceneOctreeNode;

class NSceneOctree
{
public:
	NSceneOctree();
	NSceneOctree(NScene* _scene) :NSceneOctree() { scene = _scene; }
	~NSceneOctree();

	//Build Octree Call when init Scene System
	void Build(const vector<shared_ptr<NRenderableSceneNode>>& _allSceneNodes);
	//Ray Trace Boost
	bool RayOctreeIntersect(FXMVECTOR rayPos, FXMVECTOR rayDir);
	//Frustum View Culling
	vector<shared_ptr<NRenderableSceneNode>> FrustumCulling();
	//Move Node to New Position
	void MoveNode(NRenderableSceneNode* node, Vec3 newPostion);
	//Add Node Should be called  after Scene has been almostly built
	void AddSceneNode(shared_ptr<NRenderableSceneNode> node);

private:
	XNA::AxisAlignedBox BuildAABB();
	void BuildOctree(NSceneOctreeNode* parent, const vector<shared_ptr<NRenderableSceneNode>>& nodes, int level);
	bool RayOctreeIntersect(NSceneOctreeNode* parent, FXMVECTOR rayPos, FXMVECTOR rayDir);
	NSceneOctreeNode* GetNodeToPlaceSphere(XNA::Sphere& sphere);

private:
	NSceneOctreeNode* mRoot;
	NScene* scene;
	vector<shared_ptr<NRenderableSceneNode>> allSceneNodes;
};

struct NSceneOctreeNode
{
	XNA::AxisAlignedBox Bounds;

	//if a node can't be contained into a treenode's children
	//it will be placed here
	vector<shared_ptr<NRenderableSceneNode>> sceneNodes;

	NSceneOctreeNode* Children[8];

	bool IsLeaf;

	NSceneOctreeNode()
	{
		for (int i = 0; i < 8; ++i)
			Children[i] = 0;

		Bounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
		Bounds.Extents = XMFLOAT3(0.0f, 0.0f, 0.0f);

		IsLeaf = false;
	}

	~NSceneOctreeNode()
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