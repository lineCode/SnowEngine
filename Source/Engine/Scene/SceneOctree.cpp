#include"EngineStd.h"
#include"SceneOctree.h"

SceneOctree::SceneOctree()
	: mRoot(0)
{}

SceneOctree::~SceneOctree()
{
	if (mRoot)
		delete mRoot;
}

void SceneOctree::Build(const std::vector<shared_ptr<ISceneNode>>& _allSceneNodes)
{
	// Cache a copy of the vertices.
	allSceneNodes = _allSceneNodes;

	// Build AABB to contain the scene mesh.
	XNA::AxisAlignedBox sceneBounds = BuildAABB();

	// Allocate the root node and set its AABB to contain the scene mesh.
	mRoot = new SceneOctreeNode();
	mRoot->Bounds = sceneBounds;

	BuildOctree(mRoot, allSceneNodes, 0);
}

XNA::AxisAlignedBox SceneOctree::BuildAABB()
{
	XMVECTOR vmin = XMVectorReplicate(+FLT_MAX);
	XMVECTOR vmax = XMVectorReplicate(-FLT_MIN);
	for (size_t i = 0; i < allSceneNodes.size(); ++i)
	{
		if (!allSceneNodes[i]->VGet()) continue;
		Vec3 center = allSceneNodes[i]->VGet()->ToWorld().GetPosition();
		float radius = allSceneNodes[i]->VGet()->Radius();

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
inline bool CheckBoxContain(const XNA::AxisAlignedBox& ABox,const XNA::AxisAlignedBox& BBox)
{
	Vec3 ABox_Max = Vec3(ABox.Center) + Vec3(ABox.Extents);
	Vec3 BBox_Max = Vec3(BBox.Center) + Vec3(BBox.Extents);
	Vec3 ABox_Min = Vec3(ABox.Center) - Vec3(ABox.Extents);
	Vec3 BBox_Min = Vec3(BBox.Center) - Vec3(BBox.Extents);

	if (ABox_Max >= BBox_Max && ABox_Min <= BBox_Min)
		return true;
	return false;
}

void SceneOctree::BuildOctree(SceneOctreeNode* parent,const std::vector<shared_ptr<ISceneNode>>& nodes, int level)
{
	//at most 3 level 
	if (level == 3)
	{
		parent->IsLeaf = true;
		parent->sceneNodes = nodes;
		for (auto iter = parent->sceneNodes.begin(); iter != parent->sceneNodes.end(); iter++) 
			const_cast<SceneNodeProperties*>((*iter)->VGet())->SetContainetr(parent);
	}
	else
	{
		parent->IsLeaf = false;
		XNA::AxisAlignedBox subbox[8];
		parent->Subdivide(subbox);

		for (int i = 0; i < 8; i++) 
		{
			parent->Children[i] = new SceneOctreeNode();
			parent->Children[i]->Bounds = subbox[i];
		}

		std::vector<shared_ptr<ISceneNode>> intersectedNodes[8];

		for (size_t i = 0; i < nodes.size(); i++)
		{
			const SceneNodeProperties* node = nodes[i]->VGet();
			if (!node) continue;
			for (size_t j = 0; j < 8; j++)
			{
				XNA::AxisAlignedBox object;
				Vec3 center = node->ToWorld().GetPosition();
				object.Center = XMFLOAT3(center.x, center.y, center.z);
				object.Extents = XMFLOAT3(node->Radius(), node->Radius(), node->Radius());

				XNA::AxisAlignedBox& box = subbox[j];
				if (CheckBoxContain(box, object)) 
				{
					intersectedNodes[j].push_back(nodes[i]);
				}
				else 
				{
					parent->sceneNodes.push_back(nodes[i]);
					const SceneNodeProperties* const_properties = nodes[i]->VGet();
					SceneNodeProperties* properties = const_cast<SceneNodeProperties*>(const_properties);
					properties->SetContainetr(parent);
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

bool SceneOctree::RayOctreeIntersect(FXMVECTOR rayPos, FXMVECTOR rayDir)
{
	return RayOctreeIntersect(mRoot, rayPos, rayDir);
}

bool SceneOctree::RayOctreeIntersect(SceneOctreeNode* parent, FXMVECTOR rayPos, FXMVECTOR rayDir)
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
			const SceneNodeProperties* properties = boxNodes[i]->VGet();
			XNA::Sphere sphere;
			Vec3 center = properties->FromWorld().GetPosition();

			sphere.Center = XMFLOAT3(center.x, center.y, center.z);
			sphere.Radius = properties->Radius();

			float t = 0;
			if (XNA::IntersectRaySphere(rayPos, rayDir, &sphere, &t))
				return true;
		}
		return false;
	}
}

void RecurseAddNode(std::vector<shared_ptr<ISceneNode>>& res, SceneOctreeNode* node, XNA::Frustum& frustum)
{
	if (node->sceneNodes.size() != 0) 
	{
		res.insert(res.end(), node->sceneNodes.begin(), node->sceneNodes.end());
	}
	if(node->Children[0]!=nullptr)
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

std::vector<shared_ptr<ISceneNode>> SceneOctree::FrustumCulling(CameraNode* cameraNode)
{
	std::vector<shared_ptr<ISceneNode>> res;
	if (!scene) return res;

	shared_ptr<CameraNode> camera = scene->GetCamera();
	Vec3 cameraPos = camera->GetView().Inverse().GetPosition();
	Vec3 direction = camera->GetView().GetDirection();

	Frustum frustum = camera->GetFrustum();
	XNA::Frustum xFrustum;

	xFrustum.Origin = XMFLOAT3(cameraPos.x, cameraPos.y, cameraPos.z);
	Quaternion quat;
	quat.Build(camera->GetView());
	xFrustum.Orientation= XMFLOAT4(quat.x,quat.y,quat.z,quat.w);
	xFrustum.Near = frustum.Near;
	xFrustum.Far = frustum.Far;
	float tanHalfAngle = tanf(frustum.m_Fov/2);
	xFrustum.LeftSlope = -frustum.m_Aspect * tanHalfAngle;
	xFrustum.RightSlope = frustum.m_Aspect * tanHalfAngle;
	xFrustum.TopSlope = tanHalfAngle;
	xFrustum.BottomSlope = -tanHalfAngle;

	RecurseAddNode(res, mRoot, xFrustum);

	return res;
}

SceneOctreeNode* SceneOctree::GetNodeToPlaceSphere(XNA::Sphere& sphere) 
{
	SceneOctreeNode* nodeToSearch = mRoot;
	while (true) 
	{
		//have child
		bool inChild = false;
		if (nodeToSearch->Children[0]) 
		{
			for (size_t i = 0; i < 8; i++) 
			{
				SceneOctreeNode* childNode = nodeToSearch->Children[i];
				XNA::AxisAlignedBox object;
				object.Center = XMFLOAT3(sphere.Center.x, sphere.Center.y, sphere.Center.z);
				object.Extents = XMFLOAT3(sphere.Radius, sphere.Radius, sphere.Radius);
				if (CheckBoxContain(childNode->Bounds,object)) 
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

void SceneOctree::MoveNode(ISceneNode* node,const Vec3 newPostion)
{
	if (node && node->VGet()) 
	{
		SceneOctreeNode* container = node->VGet()->GetContainer();
		float radius = node->VGet()->Radius();
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
		sphere.Radius = node->VGet()->Radius();
		SceneOctreeNode* newNodeToPlace = GetNodeToPlaceSphere(sphere);
		shared_ptr<ISceneNode> wnode;
		wnode.reset(node);
		newNodeToPlace->sceneNodes.push_back(wnode);
	}
}

void SceneOctree::AddSceneNode(shared_ptr<ISceneNode> node) 
{
	if (!node->VGet()) return;

	allSceneNodes.push_back(node);
	Vec3 min=node->VGet()->ToWorld().GetPosition()-Vec3(node->VGet()->Radius());
	Vec3 max= node->VGet()->ToWorld().GetPosition() + Vec3(node->VGet()->Radius());
	Vec3 boxMin = Vec3(mRoot->Bounds.Center) - Vec3(mRoot->Bounds.Extents);
	Vec3 boxMax = Vec3(mRoot->Bounds.Center) + Vec3(mRoot->Bounds.Extents);

	if (boxMin<min || boxMax>max) 
	{
		//rebuild bounds
		XNA::AxisAlignedBox sceneBounds = BuildAABB();
		if (mRoot) delete mRoot;

		mRoot = new SceneOctreeNode();
		mRoot->Bounds = sceneBounds;

		BuildOctree(mRoot, allSceneNodes, 0);
	}

	//add to new position
	XNA::Sphere sphere;
	Vec3 nodePosition = node->VGet()->ToWorld().GetPosition();
	sphere.Center = XMFLOAT3(nodePosition.x, nodePosition.y, nodePosition.z);
	sphere.Radius = node->VGet()->Radius();
	SceneOctreeNode* newNodeToPlace = GetNodeToPlaceSphere(sphere);
	newNodeToPlace->sceneNodes.push_back(node);
}