#include"../Msvc/EngineStd.h"
#include"NSceneNode.h"
#include"NSceneOctree.h"

NSceneNode::NSceneNode()
	:isStatic(false), actorId(INVAILD_ACOTR_ID)
{}

NSceneNode::NSceneNode(int _actorId)
	:isStatic(false), actorId(_actorId)
{}

NSceneNode::NSceneNode(int _actorId, bool _isStatic)
	:isStatic(_isStatic), actorId(_actorId)
{}

NSceneNode::NSceneNode(const string& _name, int _actorId,bool _isStatic = false)
	:NSceneNode(_actorId,_isStatic)
{
	name = _name;
}

NSceneNode::NSceneNode(const string& name,const Mat4x4& to, int actorId, bool isStatic)
	:NSceneNode(actorId,isStatic)
{
	toWorld = to;
	hasSetPos = true;
}

bool NSceneNode::VAddChild(shared_ptr<NSceneNode> kid)
{
	if (kid->parent) return false;
	if (childrenIds.find(kid->actorId) != childrenIds.end()) return false;
	
	childrens.push_back(kid);
	childrenIds.insert(kid->actorId);
	kid->parent = this;
	return true;
}

bool NSceneNode::VRemoveChild(ActorId id)
{
	auto it = childrenIds.find(id);
	if ( it== childrenIds.end()) return false;
	childrenIds.erase(it);
	for (auto iter = childrens.begin(); iter != childrens.end(); iter++)
	{
		if ((*iter)->actorId == id) {
			childrens.erase(iter);
			break;
		}
	}
	return true;
}

Vec3 NSceneNode::GetPosition() const
{
	return toWorld.GetPosition();
}

void NSceneNode::SetPosition(const Vec3& pos)
{
	if (isStatic && hasSetPos) return;
	hasSetPos = true;
	toWorld.SetPosition(pos);
}

Vec3 NSceneNode::GetDirection() const
{
	return toWorld.GetDirection();
}

void NSceneNode::SetTransform(const Mat4x4* _toWorld, const Mat4x4* _fromWorld)
{
	if (isStatic && hasSetPos) return;
	hasSetPos = true;

	toWorld = *_toWorld;
	if (!_fromWorld)
	{
		fromWorld = toWorld.Inverse();
	}
	else
	{
		fromWorld = *_fromWorld;
	}
}

bool NSceneNode::IsStatic()
{
	return isStatic;
}


NRenderableSceneNode::NRenderableSceneNode(int actorId = INVAILD_ACOTR_ID, bool isStatic = false)
	:NSceneNode(actorId, isStatic), renderType(Render::StaticMesh),
	renderPass(Render::Opaque),visable(true),container(nullptr)
{}

NRenderableSceneNode::NRenderableSceneNode(const string& name, int actorId = INVAILD_ACOTR_ID, bool isStatic = false)
	: NSceneNode(name, actorId, isStatic), renderType(Render::StaticMesh),
	renderPass(Render::Opaque), visable(true), container(nullptr)
{}

NRenderableSceneNode::NRenderableSceneNode(Render::RenderType _type, const string&, int actorId = INVAILD_ACOTR_ID, bool isStatic = false)
	: NSceneNode(name, actorId, isStatic), renderType(_type),
	renderPass(Render::Opaque), visable(true), container(nullptr)
{}

void NRenderableSceneNode::SetSphereRadius(float _radius)
{
	radius = _radius;
}

float NRenderableSceneNode::GetSphereRadius()
{
	return radius;
}

void NRenderableSceneNode::SetRenderPass(Render::RenderPass _pass)
{
	renderPass = _pass;
}

Render::RenderPass NRenderableSceneNode::GetRenderPass()
{
	return renderPass;
}

Render::RenderType NRenderableSceneNode::GetRenderType()
{
	return renderType;
}

void NRenderableSceneNode::SetContainer(NSceneOctreeNode* _container)
{
	container = _container;
}

NSceneOctreeNode* NRenderableSceneNode::GetContainer()
{
	return container;
}

NFrustum::NFrustum():nearz(1.0f),farz(1000.0f),
ratio(16.0/9.0),fov(4/GCC_PI)
{
	Init(fov, ratio, nearz, farz);
}

float NFrustum::GetFov()
{
	return fov;
}

float NFrustum::GetRatio()
{
	return ratio;
}

float NFrustum::GetNearZ()
{
	return nearz;
}

float NFrustum::GetFarZ()
{
	return farz;
}

void NFrustum::Init(float _fov, float _ratio,float _nearZ, float _farZ)
{
	fov = _fov;
	ratio = _ratio;
	nearz = _nearZ;
	farz = _farZ;
	
	float tanHalfAngle = tanf(_fov / 2);
	leftSlope = -ratio * tanHalfAngle;
	rightSlope = ratio * tanHalfAngle;
	topSlope = tanHalfAngle;
	bottomSlope = -tanHalfAngle;

}

NCameraNode::NCameraNode()
	:NSceneNode()
{
	const float pi = 3.141592653;
	Init(pi, 1.0f, 1000.0f);
}

NCameraNode::NCameraNode(float _fov, float _nearz, float _farz)
{
	Init(_fov, _nearz, _farz);
}

const NFrustum NCameraNode::GetFrustum() const
{
	return frustum;
}

const Mat4x4 NCameraNode::GetView() const
{
	return fromWorld;
}

const Mat4x4 NCameraNode::GetProjection() const
{
	return projection;
}

const Mat4x4 NCameraNode::GetViewProjection() const
{
	return fromWorld * projection;
}

void NCameraNode::Init(float _fov, float _nearz, float _farz)
{
	float _ratio = DXUTGetWindowWidth() / (float)DXUTGetWindowHeight();
	frustum.Init(_fov,_ratio, _nearz, _farz);
	D3DXMatrixPerspectiveFovLH(&projection, frustum.fov, frustum.ratio, frustum.nearz, frustum.farz);	
}

void NCameraNode::SetFov(float _fov)
{
	const float pi = 3.141592653;
	if (_fov <= 0 || _fov >= pi) return;
	Init(_fov,frustum.nearz, frustum.farz);
}

const XNA::Frustum NCameraNode::GetDXFrustum()
{
	XNA::Frustum f;
	f.BottomSlope = frustum.bottomSlope;
	f.Far = frustum.farz;
	f.LeftSlope = frustum.leftSlope;
	f.Near = frustum.nearz;
	f.RightSlope = frustum.rightSlope;
	f.TopSlope = frustum.topSlope;
	Vec3 cameraPos = GetPosition();
	f.Origin = XMFLOAT3(cameraPos.x, cameraPos.y, cameraPos.z);
	Quaternion quat;
	quat.Build(GetView());
	f.Orientation = XMFLOAT4(quat.x, quat.y, quat.z, quat.w);
	return f;
}