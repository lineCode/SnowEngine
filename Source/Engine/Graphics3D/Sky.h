#pragma once
// Sky.h - implements a sky box in either D3D9 or D3D11

#include "Geometry.h"
#include "Material.h"
#include "Shaders.h"

// Forward declarations
class SceneNode;
class Scene;

////////////////////////////////////////////////////
//
// class SkyNode					- 
//
//    Implements a believable sky that follows
//	  the camera around - this is a base class that the D3D9 and D3D11 classes 
//    inherit from
//
////////////////////////////////////////////////////

class SkyNode : public SceneNode
{
protected:
	DWORD					m_numVerts;
	DWORD					m_sides;
	const char *			m_textureBaseName;
	shared_ptr<CameraNode>	m_camera;
	bool					m_bActive;

	std::string GetTextureName(const int side);

public:
	SkyNode(const char *textureFile);
	virtual ~SkyNode() { }
	HRESULT VPreRender(Scene *pScene);
	bool VIsVisible(Scene *pScene) const { return m_bActive; }
};


class D3DSkyNode9 : public SkyNode
{
protected:
	LPDIRECT3DTEXTURE9		m_pTexture[5];		// the sky textures
	LPDIRECT3DVERTEXBUFFER9 m_pVerts;			// the sky verts

public:
	D3DSkyNode9(const char *pTextureBaseName );
	virtual ~D3DSkyNode9();
	HRESULT VOnRestore(Scene *pScene);
	HRESULT VRender(Scene *pScene);
};


class D3DSkyNode11 : public SkyNode
{
public:
	D3DSkyNode11(const char *pTextureBaseName );
	virtual ~D3DSkyNode11();
	HRESULT VOnRestore(Scene *pScene);
	HRESULT VRender(Scene *pScene);

protected:
	ID3D11Buffer*               m_pIndexBuffer;
	ID3D11Buffer*               m_pVertexBuffer;

	VertexShader_HLSL		m_VertexShader;
	PixelShader_HLSL		m_PixelShader;
};

