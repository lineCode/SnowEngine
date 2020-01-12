#pragma once
// EditorHumanView.h : Editor's Human View

#include "..\..\Engine\Scene\SceneNodes.h"
#include "..\..\Engine\EventManager\EventManager.h"
#include "..\..\Engine\UserInterface\HumanView.h"

//========================================================================
//
// This will be similar to TeapotWarsView, but will be much more 
// streamlined since we don't a HUD, or a main menu screen, or sound 
// controllers.
//
//========================================================================

class EditorController;

class EditorHumanView : public HumanView
{
public:

	EditorHumanView(shared_ptr<IRenderer> renderer); 
	virtual ~EditorHumanView() {  }
	virtual void VOnUpdate(unsigned long deltaMilliseconds);
	shared_ptr<ScreenElementScene> GetScene()	{ return m_pScene; }

protected:
	shared_ptr<MovementController> m_pFreeCameraController;

	virtual bool VLoadGameDelegate(TiXmlElement* pLevelData) override;
};
