#pragma once
// File: Scene.h - implements the container class for 3D Graphics scenes

#include "../Graphics3D/Geometry.h"
#include "SceneNodes.h"

typedef std::map<ActorId, shared_ptr<ISceneNode> > SceneActorMap;

class CameraNode;
class SkyNode;
class LightNode;
class LightManager;
class SceneMeshNode;

class Scene
{
protected:
	shared_ptr<SceneNode>  	m_Root;
	shared_ptr<CameraNode> 	m_Camera;
	shared_ptr<IRenderer>	m_Renderer;

	ID3DXMatrixStack 		*m_MatrixStack;
	AlphaSceneNodeList 		m_AlphaSceneNodes;
	SceneActorMap 			m_ActorMap;

	LightManager			*m_LightManager;

	void RenderAlphaPass();

public:
	Scene(shared_ptr<IRenderer> renderer);
	virtual ~Scene();

	HRESULT OnRender();
	HRESULT OnRestore();
	HRESULT OnLostDevice();
	HRESULT OnUpdate(const int deltaMilliseconds);
	shared_ptr<ISceneNode> FindActor(ActorId id);
	bool AddChild(ActorId id, shared_ptr<ISceneNode> kid);
	bool RemoveChild(ActorId id);

    // event delegates
    void NewRenderComponentDelegate(IEventDataPtr pEventData);
    void ModifiedRenderComponentDelegate(IEventDataPtr pEventData);			// added post-press!
    void DestroyActorDelegate(IEventDataPtr pEventData);
    void MoveActorDelegate(IEventDataPtr pEventData);

	void SetCamera(shared_ptr<CameraNode> camera) { m_Camera = camera; }
	const shared_ptr<CameraNode> GetCamera() const { return m_Camera; }


	void PushAndSetMatrix(const Mat4x4 &toWorld)
	{
		// Note this code carefully!!!!! It is COMPLETELY different
		// from some DirectX 9 documentation out there....
		// Scene::PushAndSetMatrix - 

		m_MatrixStack->Push();
		m_MatrixStack->MultMatrixLocal(&toWorld);
		Mat4x4 mat = GetTopMatrix();
		m_Renderer->VSetWorldTransform(&mat);
	}

	void PopMatrix() 
	{
		// Scene::PopMatrix - 
		m_MatrixStack->Pop(); 
		Mat4x4 mat = GetTopMatrix();
		m_Renderer->VSetWorldTransform(&mat);
	}

	const Mat4x4 GetTopMatrix() 
	{ 
		// Scene::GetTopMatrix - 
		return static_cast<const Mat4x4>(*m_MatrixStack->GetTop()); 
	}

	LightManager *GetLightManager() { return m_LightManager; }

	void AddAlphaSceneNode(AlphaSceneNode *asn) { m_AlphaSceneNodes.push_back(asn); }

	HRESULT Pick(RayCast *pRayCast) { return m_Root->VPick(this, pRayCast); }

	shared_ptr<IRenderer> GetRenderer() { return m_Renderer; }

};

