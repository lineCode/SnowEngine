#pragma once
// Shaders.h - helper classes to call GameCode4_VS.hlsl and GameCode4_PS.hlsl

//========================================================================
// Content References in Game Coding Complete 4th Edition
// 
// class VertexShader_HLSL			
// class PixelShader_HLSL			


#include <xnamath.h>

#include "Geometry.h"
#include "Material.h"

// Forward declarations
class SceneNode;
class Scene;

class VertexShader_HLSL
{
public:
	VertexShader_HLSL();
	~VertexShader_HLSL();

	HRESULT OnRestore(Scene *pScene);
	HRESULT SetupRender(Scene *pScene, SceneNode *pNode);
	void EnableLights(bool enableLights) { m_enableLights = enableLights; }

protected:
	ID3D11InputLayout*          m_pVertexLayout11;
	ID3D11VertexShader*         m_pVertexShader;
	ID3D11Buffer*               m_pcbVSMatrices;
	ID3D11Buffer*               m_pcbVSLighting;
	ID3D11Buffer*               m_pcbVSMaterial;
	bool						m_enableLights;
};

class PixelShader_HLSL
{
public:
	PixelShader_HLSL();
	~PixelShader_HLSL();

	HRESULT OnRestore(Scene *pScene);
	HRESULT SetupRender(Scene *pScene, SceneNode *pNode);
	HRESULT SetTexture(const std::string& textureName);
	HRESULT SetTexture(ID3D11ShaderResourceView* const *pDiffuseRV, ID3D11SamplerState * const *ppSamplers);

protected:
	ID3D11PixelShader*          m_pPixelShader;
	ID3D11Buffer*               m_pcbPSMaterial;
	std::string					m_textureResource;
};

struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectPass;

class LineDraw_Hlsl_Shader
{
public:
	LineDraw_Hlsl_Shader();
	~LineDraw_Hlsl_Shader();

	HRESULT OnRestore(Scene *pScene);
	HRESULT SetupRender(Scene *pScene);
	HRESULT SetDiffuse(const std::string& textureName, const Color &color);
	HRESULT SetTexture(ID3D11ShaderResourceView* const *pDiffuseRV, ID3D11SamplerState * const *ppSamplers);

protected:
	ID3D11InputLayout*          m_pVertexLayout11;
	ID3D11Buffer*               m_pcbRenderTargetSize;
	ID3DX11Effect*				m_pEffect;

	ID3DX11EffectTechnique*		m_EffectTechnique; // No need to be Release()-d.
	ID3DX11EffectPass*			m_EffectPass; // No need to be Release()-d.

	ID3D11Buffer*               m_pcbChangePerFrame;

	ID3D11Buffer*               m_pcbDiffuseColor;

	std::string					m_textureResource;

};
