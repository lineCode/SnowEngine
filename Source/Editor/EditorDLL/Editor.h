#pragma once
// Editor.h : source file for the editor application and logic in C++

//========================================================================
//  Content References in Game Coding Complete 4th Edition
// 
//  class Editor						- 
//  class EditorLogic					- 
//

#include "..\..\Engine\Engine\Engine.h"

#include "..\..\Engine\Physics\Physics.h"
#include "..\..\Engine\Physics\PhysicsEventListener.h"
#include "..\..\Engine\MainLoop\Initialization.h"

#include "..\..\Engine\EventManager\Events.h"
#include "..\..\Engine\EventManager\EventManagerImpl.h"

#include "EditorHumanView.h"

using namespace std;

class EditorLogic;

class EditorApp : public Engine
{
public:
	EditorApp() : Engine() { m_bIsEditorRunning = true; }
	TCHAR *VGetGameTitle() { return _T("GameCode4 Editor"); }
	TCHAR *VGetGameAppDirectory() { return _T("Game Coding Complete 4\\Editor\\1.0"); }
	HICON VGetIcon();

protected:
	BaseGameLogic *VCreateGameAndView();
};


class EditorLogic : public BaseGameLogic
{
public:
	EditorLogic();
	~EditorLogic();
	
	virtual bool VLoadGame(const char* levelName);
	const std::string &GetProjectDirectory(void) { return m_ProjectDirectory; }

	// We need to expose this information so that the C# app can
	// know how big of an array to allocate to hold the list of
	// actors
	int GetNumActors() { return (int)m_actors.size(); }

	// Exposes the actor map so that the global functions
	// can retrieve actor information
	const ActorMap& GetActorMap() { return m_actors; }

	bool IsRunning() { return (BGS_Running == m_State); }
	shared_ptr<EditorHumanView> GetHumanView();

protected:
	std::string m_ProjectDirectory;
};

