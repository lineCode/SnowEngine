// EditorHumanView.cpp : Editor's Human View
#include "../Msvc/EditorStd.h"

#include "..\..\Engine\Engine\Engine.h"
#include "..\..\Engine\Actors\Actor.h"
#include "..\..\Engine\Actors\RenderComponent.h"
#include "..\..\Engine\Audio\Audio.h"
#include "..\..\Engine\Audio\SoundProcess.h"
#include "..\..\Engine\Scene\SceneNodes.h"
#include "..\..\Engine\Graphics3D\Geometry.h"
#include "..\..\Engine\Graphics3D\Sky.h"
#include "..\..\Engine\Graphics3D\MovementController.h"
#include "..\..\Engine\EventManager\Events.h"
#include "..\..\Engine\EventManager\EventManagerImpl.h"
#include "..\..\Engine\Physics\PhysicsEventListener.h"

#include "EditorHumanView.h"
#include "Editor.h"

extern EditorApp globalApp;

//========================================================================
//
// EditorHumanView implementation
//
//========================================================================

//
// EditorHumanView::EditorHumanView				- 
//
EditorHumanView::EditorHumanView(shared_ptr<IRenderer> renderer) 
	: HumanView(renderer)
{
	// The EditorHumanView differs from its counterpart in TeapotWars primarily
	// in the fact that we don't have any UI classes. Any level information
	// should be displayed in the C# app.
}

//
// EditorHumanView::VOnUpdate					- 
//
void EditorHumanView::VOnUpdate( unsigned long deltaMilliseconds )
{
	// Much like TeapotWarsView::VOnUpdate, except
	// we only have one controller in the editor

	HumanView::VOnUpdate( deltaMilliseconds );

	if (m_pFreeCameraController)
	{
		m_pFreeCameraController->OnUpdate(deltaMilliseconds);
	}
}

//
// EditorHumanView::VLoadGameDelegate			- 
//
bool EditorHumanView::VLoadGameDelegate(TiXmlElement* pLevelData)
{
	if (!HumanView::VLoadGameDelegate(pLevelData))
		return false;

	// We also make sure that the MovementController is hooked up
	// to the keyboard and mouse handlers, since this is our primary method
	// for moving the camera around.
	m_pFreeCameraController.reset(GCC_NEW MovementController(m_pCamera, 90, 0, true));
	m_pCamera->ClearTarget();

	m_KeyboardHandler = m_pFreeCameraController;
	m_PointerHandler = m_pFreeCameraController;

    //  This needs to be here for you get a black screen
    m_pScene->VOnRestore();
    return true;
}
