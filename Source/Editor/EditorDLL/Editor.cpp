// Editor.cpp : source file for the editor application and logic in C++

#include "../Msvc/EditorStd.h"

#include "resource.h"
#include "Editor.h"

#include "../../Engine/Actors/Actor.h"
#include "../../Engine/Actors/TransformComponent.h"
#include "../../Engine/AI/Pathing.h"
#include "../../Engine/EventManager/Events.h"
#include "../../Engine/EventManager/EventManagerImpl.h"
#include "../../Engine/Engine/Engine.h"
#include "../../Engine/Physics/Physics.h"
#include "../../Engine/Physics/PhysicsEventListener.h"
#include "../../Engine/MainLoop/Initialization.h"


#include "../../Engine/Utilities/String.h"

#include <direct.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")			// useful for Windows Registry queries
#pragma comment(lib, "OleAut32.lib")			// needed for the C# / C++ BSTR conversions

#pragma comment(lib, "dxut.lib")
#pragma comment(lib, "dxutopt.lib")


#if defined(_M_IX86)
	#if defined(_DEBUG)
		#pragma comment(lib, "bulletcollision_debug.lib")
		#pragma comment(lib, "bulletdynamics_debug.lib")
		#pragma comment(lib, "linearmath_debug.lib")
	#else
		#pragma comment(lib, "bulletcollision.lib")
		#pragma comment(lib, "bulletdynamics.lib")
		#pragma comment(lib, "linearmath.lib")
	#endif
#elif defined(_M_X64)
	#if defined(_DEBUG)
		#pragma comment(lib, "bulletcollision_x64_debug.lib")
		#pragma comment(lib, "bulletdynamics_x64_debug.lib")
		#pragma comment(lib, "linearmath_x64_debug.lib")
	#else
		#pragma comment(lib, "bulletcollision_x64.lib")
		#pragma comment(lib, "bulletdynamics_x64.lib")
		#pragma comment(lib, "linearmath_x64.lib")
	#endif
#else
	#error Preprocessor defines can't figure out which Bullet library to use.
#endif



#pragma comment(lib, "zlibstat.lib")

#pragma comment(lib, "libogg_static.lib")
#pragma comment(lib, "libvorbis_static.lib")
#pragma comment(lib, "libvorbisfile_static.lib")

#pragma comment(lib, "tinyxml.lib")

#pragma comment(lib, "legacy_stdio_definitions.lib")


EditorApp globalApp;

//========================================================================
//
// Editor Implementation
//
//========================================================================

//
// EditorApp::VCreateGameAndView						- 
//
BaseGameLogic* EditorApp::VCreateGameAndView()
{
	BaseGameLogic *game = NULL;

	game = GCC_NEW EditorLogic();
    game->Init();

	shared_ptr<IGameView> gameView(GCC_NEW EditorHumanView(g_pApp->m_Renderer));
	game->VAddView(gameView);

	return game;
}

HICON EditorApp::VGetIcon()
{
	return LoadIcon(GetInstance(), MAKEINTRESOURCE(IDI_ICON1));
}


//========================================================================
//
// EditorLogic Implementation
//
//========================================================================

//
// EditorLogic::EditorLogic					- 
//
EditorLogic::EditorLogic()
: BaseGameLogic()
{
	// FUTURE WORK - this is a little hardcoded - but since the editor is built along with the 
	//    framework it is probably ok to assume a current working directory.
	m_ProjectDirectory = getcwd(NULL, 0);
	int slashGamePos = m_ProjectDirectory.rfind("\\Game");
	m_ProjectDirectory = m_ProjectDirectory.substr(0, slashGamePos);

	// We're not using physics for the editor, but as an optional feature,
	// you can hook up a control that will allow you to toggle the physics
	// on and off. I leave this as a future assignment!
	m_pPhysics.reset(CreateNullPhysics());
}


//
// EditorLogic::~EditorLogic				- Chapter 22, not in the book
//
EditorLogic::~EditorLogic()
{

}

//
// EditorLogic::VLoadGame					- 
//
bool EditorLogic::VLoadGame(const char* levelName)
{
	while (m_actors.size() > 0)
	{
		ActorId id = m_actors.begin()->first;
		VDestroyActor(id);
	}

	if (!BaseGameLogic::VLoadGame(levelName))
	{
		return false;	
	}

	VChangeState(BGS_Running);
	return true;
}

//
// EditorLogic::GetHumanView					- 
//
shared_ptr<EditorHumanView> EditorLogic::GetHumanView() 
{
	GCC_ASSERT(m_gameViews.size()==1);
	shared_ptr<IGameView> pGameView = *m_gameViews.begin();
	shared_ptr<EditorHumanView> editorHumanView = static_pointer_cast<EditorHumanView>( pGameView );
	return editorHumanView; 
}
