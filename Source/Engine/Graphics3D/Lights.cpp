// Lights.h - implements a simple light class

#include "EngineStd.h"

#include "../Engine/Engine.h"
#include "../Actors/RenderComponent.h"
#include "Lights.h"


LightNode::LightNode(const ActorId actorId, WeakBaseRenderComponentPtr renderComponent, const LightProperties &props,  const Mat4x4 *t)
 : SceneNode(actorId, renderComponent,  RenderPass_NotRendered,  t) 
{
	m_LightProps = props;
}

HRESULT D3DLightNode9::VOnRestore(Scene *pScene)
{
	ZeroMemory( &m_d3dLight9, sizeof(D3DLIGHT9) );
    m_d3dLight9.Type        = D3DLIGHT_DIRECTIONAL;

	// These parameters are constant for the list after the scene is loaded
	m_d3dLight9.Range        = m_LightProps.m_Range;
    m_d3dLight9.Falloff		= m_LightProps.m_Falloff;
	m_d3dLight9.Attenuation0	= m_LightProps.m_Attenuation[0];
    m_d3dLight9.Attenuation1	= m_LightProps.m_Attenuation[0];
    m_d3dLight9.Attenuation2	= m_LightProps.m_Attenuation[0];
    m_d3dLight9.Theta			= m_LightProps.m_Theta;
    m_d3dLight9.Phi			= m_LightProps.m_Phi;

	return S_OK;
}


HRESULT D3DLightNode9::VOnUpdate(Scene *, DWORD const elapsedMs)
{
	// light color can change anytime! Check the BaseRenderComponent!
	LightRenderComponent* lrc = static_cast<LightRenderComponent*>(m_RenderComponent);
	m_Props.GetMaterial().SetDiffuse(lrc->GetColor());

	m_d3dLight9.Diffuse = m_Props.GetMaterial().GetDiffuse();
	float power;
	Color spec;
    m_Props.GetMaterial().GetSpecular(spec, power);
	m_d3dLight9.Specular = spec;
    m_d3dLight9.Ambient = m_Props.GetMaterial().GetAmbient();

	m_d3dLight9.Position = GetPosition();
	m_d3dLight9.Direction = GetDirection();
	return S_OK;
}

HRESULT D3DLightNode11::VOnUpdate(Scene *, DWORD const elapsedMs)
{ 
	// light color can change anytime! Check the BaseRenderComponent!
	LightRenderComponent* lrc = static_cast<LightRenderComponent*>(m_RenderComponent);
	m_Props.GetMaterial().SetDiffuse(lrc->GetColor());
	return S_OK; 
}


//
// LightManager::CalcLighting					- 
//
void LightManager::CalcLighting(Scene *pScene)
{
	// FUTURE WORK: There might be all kinds of things you'd want to do here for optimization, especially turning off lights on actors that can't be seen, etc.
	pScene->GetRenderer()->VCalcLighting(&m_Lights, MAXIMUM_LIGHTS_SUPPORTED);

	int count = 0;

	GCC_ASSERT(m_Lights.size() < MAXIMUM_LIGHTS_SUPPORTED);
	for(Lights::iterator i=m_Lights.begin(); i!=m_Lights.end(); ++i, ++count)
	{
		shared_ptr<LightNode> light = *i;

		if (count==0)
		{
			// Light 0 is the only one we use for ambient lighting. The rest are ignored in the simple shaders
			Color ambient = light->VGet()->GetMaterial().GetAmbient();
			m_vLightAmbient = D3DXVECTOR4(ambient.r, ambient.g, ambient.b, 1.0f); 		
		}

		Vec3 lightDir = light->GetDirection();
		m_vLightDir[count] = D3DXVECTOR4(lightDir.x, lightDir.y, lightDir.z, 1.0f);
		m_vLightDiffuse[count] = light->VGet()->GetMaterial().GetDiffuse();
	}
}


void LightManager::CalcLighting(ConstantBuffer_Lighting* pLighting, SceneNode *pNode)
{
	int count = GetLightCount(pNode);
	if (count)
	{
		pLighting->m_vLightAmbient = *GetLightAmbient(pNode);
		memcpy(pLighting->m_vLightDir, GetLightDirection(pNode), sizeof( Vec4 ) * count );
		memcpy(pLighting->m_vLightDiffuse, GetLightDiffuse(pNode), sizeof( Vec4 ) * count);
		pLighting->m_nNumLights = count;
	}
}

