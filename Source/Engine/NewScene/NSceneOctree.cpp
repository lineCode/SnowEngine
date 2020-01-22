#include"../Msvc/EngineStd.h"
#include"NSceneOctree.h"
#include"NSceneNode.h"

NSceneOctree::NSceneOctree()
	: mRoot(0)
{}

NSceneOctree::~NSceneOctree()
{
	if (mRoot)
		delete mRoot;
}

void NSceneOctree::Build(const vector<shared_ptr<NRenderableSceneNode>>& _allSceneNodes)
{
	// Cache a copy of the vertices.
	allSceneNodes = _allSceneNodes;

	// Build AABB to contain the scene mesh.
	XNA::AxisAlignedBox sceneBounds = BuildAABB();

	// Allocate the root node and set its AABB to contain the scene mesh.
	mRoot = new NSceneOctreeNode();
	mRoot->Bounds = sceneBounds;

	BuildOctree(mRoot, allSceneNodes, 0);
}

XNA::AxisAlignedBox NSceneOctree::BuildAABB()
{
	XMVECTOR vmin = XMVectorReplicate(+FLT_MAX);
	XMVECTOR vmax = XMVectorReplicate(-FLT_MIN);
	for (size_t i = 0; i < allSceneNodes.size(); ++i)
	{
		Vec3 center = allSceneNodes[i]->GetPosition();
		float radius = allSceneNodes[i]->GetSphereRadius();

		XMFLOAT3 centers(center.x, center.y, center.z);
		XMFLOAT3 extends(radius, radius, radius);

		XMVECTOR CP = XMLoadFloat3(&centers);
		XMVECTOR EP = XMLoadFloat3(&extends);

		vmin = XMVectorMin(vmin, CP - EP);
		vmax = XMVectorMax(vmax, CP + EP);
	}

	XNA::AxisAlignedBox bounds;
	XMVECTOR C = 0.5f * (vmin + vmax);
	XMVECTOR E = 0.5f * (vmax - vmin);

	XMStoreFloat3(&bounds.Center, C);
	XMStoreFloat3(&bounds.Extents, E);

	return bounds;
}

//check if ABOX contains BBOX
inline bool CheckBoxContain(const XNA::AxisAlignedBox& ABox, const XNA::AxisAlignedBox& BBox)
{
	Vec3 ABox_Max = Vec3(ABox.Center) + Vec3(ABox.Extents);
	Vec3 BBox_Max = Vec3(BBox.Center) + Vec3(BBox.Extents);
	Vec3 ABox_Min = Vec3(ABox.Center) - Vec3(ABox.Extents);
	Vec3 BBox_Min = Vec3(BBox.Center) - Vec3(BBox.Extents);

	if (ABox_Max >= BBox_Max && ABox_Min <= BBox_Min)
		return true;
	return false;
}

void NSceneOctree::BuildOctree(NSceneOctreeNode* parent, const vector<shared_ptr<NRenderableSceneNode>>& nodes, int level)
{
	//at most 3 level 
	if (level == 3)
	{
		parent->IsLeaf = true;
		parent->sceneNodes = nodes;
		for (auto iter = parent->sceneNodes.begin(); iter != parent->sceneNodes.end(); iter++)
			(*iter)->SetContainer(parent);
	}
	else
	{
		parent->IsLeaf = false;
		XNA::AxisAlignedBox subbox[8];
		parent->Subdivide(subbox);

		for (int i = 0; i < 8; i++)
		{
			parent->Children[i] = new NSceneOctreeNode();
			parent->Children[i]->Bounds = subbox[i];
		}

		vector<shared_ptr<NRenderableSceneNode>> intersectedNodes[8];

		for (size_t i = 0; i < nodes.size(); i++)
		{
			for (size_t j = 0; j < 8; j++)
			{
				XNA::AxisAlignedBox object;
				Vec3 center = nodes[i]->GetPosition();
				float radius = nodes[i]->GetSphereRadius();
				object.Center = XMFLOAT3(center.x, center.y, center.z);
				object.Extents = XMFLOAT3(radius, radius, radius);

				XNA::AxisAlignedBox& box = subbox[j];
				if (CheckBoxContain(box, object))
				{
					intersectedNodes[j].push_back(nodes[i]);
				}
				else
				{
					parent->sceneNodes.push_back(nodes[i]);
					nodes[i]->SetContainer(parent);
				}
			}
		}
		// Recurse.
		for (int i = 0; i < 8; i++)
		{
			BuildOctree(parent->Children[i], intersectedNodes[i], level++);
		}
	}
}

bool NSceneOctree::RayOctreeIntersect(FXMVECTOR rayPos, FXMVECTOR rayDir)
{
	return RayOctreeIntersect(mRoot, rayPos, rayDir);
}

bool NSceneOctree::RayOctreeIntersect(NSceneOctreeNode* parent, FXMVECTOR rayPos, FXMVECTOR rayDir)
{
	if (!parent->IsLeaf)
	{
		for (size_t i = 0; i < 8; ++i)
		{
			// Recurse down this node if the ray hit the child's box.
			float t;
			if (XNA::IntersectRayAxisAlignedBox(rayPos, rayDir, &parent->Children[i]->Bounds, &t))
			{
				//Note that we can't get the closest object yet
				if (RayOctreeIntersect(parent->Children[i], rayPos, rayDir))
					return true;
			}
		}
		return false;
	}
	else
	{
		const auto& boxNodes = parent->sceneNodes;
		for (size_t i = 0; i < boxNodes.size(); i++) {
			XNA::Sphere sphere;
			Vec3 center =boxNodes[i]->GetPosition();

			sphere.Center = XMFLOAT3(center.x, center.y, center.z);
			sphere.Radius = boxNodes[i]->GetSphereRadius();

			float t = 0;
			if (XNA::IntersectRaySphere(rayPos, rayDir, &sphere, &t))
				return true;
		}
		return false;
	}
}

void RecurseAddNode(vector<shared_ptr<NRenderableSceneNode>>& res, NSceneOctreeNode* node, XNA::Frustum& frustum)
{
	if (node->sceneNodes.size() != 0)
	{
		res.insert(res.end(), node->sceneNodes.begin(), node->sceneNodes.end());
	}
	if (node->Children[0] != nullptr)
	{
		for (size_t i = 0; i < 8; i++)
		{
			XNA::AxisAlignedBox box = node->Children[i]->Bounds;
			if (XNA::IntersectAxisAlignedBoxFrustum(&box, &frustum))
			{
				RecurseAddNode(res, node->Children[i], frustum);
			}
		}
	}
}

vector<shared_ptr<NRenderableSceneNode>> NSceneOctree::FrustumCulling()
{
	vector<shared_ptr<NRenderableSceneNode>> res;
	if (!scene) return res;

	shared_ptr<NCameraNode> camera = scene->GetCamera();
	Vec3 cameraPos = camera->GetView().Inverse().GetPosition();
	Vec3 direction = camera->GetView().GetDirection();

	Frustum frustum = camera->GetFrustum();
	XNA::Frustum xFrustum;

	xFrustum.Origin = XMFLOAT3(cameraPos.x, cameraPos.y, cameraPos.z);
	Quaternion quat;
	quat.Build(camera->GetView());
	xFrustum.Orientation = XMFLOAT4(quat.x, quat.y, quat.z, quat.w);
	xFrustum.Near = frustum.Near;
	xFrustum.Far = frustum.Far;
	float tanHalfAngle = tanf(frustum.m_Fov / 2);
	xFrustum.LeftSlope = -frustum.m_Aspect * tanHalfAngle;
	xFrustum.RightSlope = frustum.m_Aspect * tanHalfAngle;
	xFrustum.TopSlope = tanHalfAngle;
	xFrustum.BottomSlope = -tanHalfAngle;

	RecurseAddNode(res, mRoot, xFrustum);

	return res;
}

NSceneOctreeNode* NSceneOctree::GetNodeToPlaceSphere(XNA::Sphere& sphere)
{
	NSceneOctreeNode* nodeToSearch = mRoot;
	while (true)
	{
		//have child
		bool inChild = false;
		if (nodeToSearch->Children[0])
		{
			for (size_t i = 0; i < 8; i++)
			{
				NSceneOctreeNode* childNode = nodeToSearch->Children[i];
				XNA::AxisAlignedBox object;
				object.Center = XMFLOAT3(sphere.Center.x, sphere.Center.y, sphere.Center.z);
				object.Extents = XMFLOAT3(sphere.Radius, sphere.Radius, sphere.Radius);
				if (CheckBoxContain(childNode->Bounds, object))
				{
					inChild = true;
					nodeToSearch = childNode;
					break;
				}
			}
		}
		if (!inChild)
			break;
	}
	return nodeToSearch;
}

void NSceneOctree::MoveNode(NRenderableSceneNode* node, const Vec3 newPostion)
{
	if (!node) return;
	NSceneOctreeNode* container = node->GetContainer();
	float radius = node->GetSphereRadius();
	if (!container) return;
	Vec3 containerMin = Vec3(container->Bounds.Center) - Vec3(container->Bounds.Extents);
	Vec3 containerMax = Vec3(container->Bounds.Center) + Vec3(container->Bounds.Extents);

	//if still in old container...
	if (newPostion + Vec3(radius, radius, radius) <= containerMax && newPostion - Vec3(radius, radius, radius) >= containerMin) return;

	//delete from container may have many cost
	decltype(container->sceneNodes.begin()) nodeToDelete;
	for (auto iter = container->sceneNodes.begin(); iter != container->sceneNodes.end(); iter++)
	{
		if ((*iter)->VGet() == node->VGet())
		{
			//assert that we shall find an node to delete
			nodeToDelete = iter;
		}
	}
	container->sceneNodes.erase(nodeToDelete);

	//search from root since there are only 3 levels, there's no need to search from brother
	XNA::Sphere sphere;
	sphere.Center = XMFLOAT3(newPostion.x, newPostion.y, newPostion.z);
	sphere.Radius = node->GetSphereRadius();
	NSceneOctreeNode* newNodeToPlace = GetNodeToPlaceSphere(sphere);
	shared_ptr<NRenderableSceneNode> wnode;
	wnode.reset(node);
	newNodeToPlace->sceneNodes.push_back(wnode);
}

void NSceneOctree::AddSceneNode(shared_ptr<NRenderableSceneNode> node)
{
	allSceneNodes.push_back(node);
	Vec3 min = node->GetPosition() - Vec3(node->GetSphereRadius());
	Vec3 max = node->GetPosition() + Vec3(node->GetSphereRadius());
	Vec3 boxMin = Vec3(mRoot->Bounds.Center) - Vec3(mRoot->Bounds.Extents);
	Vec3 boxMax = Vec3(mRoot->Bounds.Center) + Vec3(mRoot->Bounds.Extents);

	if (boxMin<min || boxMax>max)
	{
		//rebuild bounds
		XNA::AxisAlignedBox sceneBounds = BuildAABB();
		if (mRoot) delete mRoot;

		mRoot = new NSceneOctreeNode();
		mRoot->Bounds = sceneBounds;

		BuildOctree(mRoot, allSceneNodes, 0);
	}

	//add to new position
	XNA::Sphere sphere;
	Vec3 nodePosition = node->GetPosition();
	sphere.Center = XMFLOAT3(nodePosition.x, nodePosition.y, nodePosition.z);
	sphere.Radius = node->GetSphereRadius();
	NSceneOctreeNode* newNodeToPlace = GetNodeToPlaceSphere(sphere);
	newNodeToPlace->sceneNodes.push_back(node);
}
