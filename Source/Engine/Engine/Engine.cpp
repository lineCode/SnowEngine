//Engine Core
//Last Modified On 19/12/15 By Alevery

#pragma region Header And Macro

#include "EngineStd.h"
//Required for DBT_DEVICEREMOVECOMPLETE
#include "Dbt.h"						
#include "../MainLoop/Initialization.h"
#include "../Debugging/MiniDump.h"
#include "../Engine/BaseGameLogic.h"
#include "../Graphics3D/D3DRenderer.h"
#include "../EventManager/EventManagerImpl.h"
#include "../Network/Network.h"
#include "../Script/LuaStateManager.h"
#include "../Script/ScriptExports.h"
#include "../Script/ScriptProcess.h"
#include "../ResourceCache/ResCache.h"
#include "../ResourceCache/XmlResource.h"
#include "../UserInterface/UserInterface.h"
#include "../UserInterface/MessageBox.h"
#include "../UserInterface/HumanView.h"
#include "../Utilities/Math.h"
#include "../Utilities/String.h"
#include "../Actors/BaseScriptComponent.h"
//Event
#include "../Physics/PhysicsEventListener.h"
#include "../EventManager/Events.h"

#define MAX_LOADSTRING 100
Engine *g_pApp = NULL;
const char* SCRIPT_PREINIT_FILE = "Scripts\\PreInit.lua";
#pragma endregion

#pragma region Engine Init

//实例设置
Engine::Engine()
{
	g_pApp = this;
	m_pGame = NULL;

	m_rcDesktop.bottom = m_rcDesktop.left = m_rcDesktop.right = m_rcDesktop.top = 0;
	m_screenSize = Point(0, 0);
	m_iColorDepth = 32;

	m_bIsRunning = false;
	m_bIsEditorRunning = false;

	m_pEventManager = NULL;
	m_ResCache = NULL;

	m_pNetworkEventForwarder = NULL;
	m_pBaseSocketManager = NULL;

	m_bQuitRequested = false;
	m_bQuitting = false;
	m_HasModalDialog = 0;
}

HWND Engine::GetHwnd()
{
	return DXUTGetHWND();
}

//引擎初始化工作 完成设备检查 加载缓存系统 加载脚本系统 加载事件管理器等工作
bool Engine::InitInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd, int screenWidth, int screenHeight)
{
	// Check for existing instance of the same window
	// 
#ifndef _DEBUG
	// Note - it can be really useful to debug network code to have
	// more than one instance of the game up at one time - so
	// feel free to comment these lines in or out as you wish!
	if (!IsOnlyInstance(VGetGameTitle()))
		return false;
#endif

	// We don't need a mouse cursor by default, let the game turn it on
	SetCursor( NULL );

	//检查平台性能
	bool resourceCheck = false;
	while (!resourceCheck)
	{
		const DWORDLONG physicalRAM = 512 * MEGABYTE;
		const DWORDLONG virtualRAM = 1024 * MEGABYTE;
		const DWORDLONG diskSpace = 10 * MEGABYTE;
		if (!CheckStorage(diskSpace))
			return false;

		const DWORD minCpuSpeed = 1300;			// 1.3Ghz
		DWORD thisCPU = ReadCPUSpeed();
		if (thisCPU < minCpuSpeed)
		{
			GCC_ERROR("GetCPUSpeed reports CPU is too slow for this game.");
			return false;
		}

		resourceCheck = true;
	}

	m_hInstance = hInstance;

    //注册事件
    RegisterEngineEvents();
    VRegisterGameEvents();

	//加载压缩的资源文件
	//Note:编辑器模式使用非压缩的文件以提高性能
	IResourceFile *zipFile = (m_bIsEditorRunning || m_Options.m_useDevelopmentDirectories) ? 
		GCC_NEW DevelopmentResourceZipFile(L"Assets.zip", DevelopmentResourceZipFile::Editor) :
		GCC_NEW ResourceZipFile(L"Assets.zip");

	//资源缓存器，缓存资源，初始值设为50M
	m_ResCache = GCC_NEW ResCache(50, zipFile);

	if (!m_ResCache->Init())
	{
        GCC_ERROR("Failed to initialize resource cache!  Are your paths set up correctly?");
		return false;
	}
	
	extern shared_ptr<IResourceLoader> CreateWAVResourceLoader();
	extern shared_ptr<IResourceLoader> CreateOGGResourceLoader();
	extern shared_ptr<IResourceLoader> CreateDDSResourceLoader();
	extern shared_ptr<IResourceLoader> CreateJPGResourceLoader();
    extern shared_ptr<IResourceLoader> CreateXmlResourceLoader();
    extern shared_ptr<IResourceLoader> CreateSdkMeshResourceLoader();
    extern shared_ptr<IResourceLoader> CreateScriptResourceLoader();

	//对于不同种类的资源有不同的加载器，加载器会对资源进行一定的处理，例如解码等
	m_ResCache->RegisterLoader(CreateWAVResourceLoader());
	m_ResCache->RegisterLoader(CreateOGGResourceLoader());
	m_ResCache->RegisterLoader(CreateDDSResourceLoader());
	m_ResCache->RegisterLoader(CreateJPGResourceLoader());
    m_ResCache->RegisterLoader(CreateXmlResourceLoader());
	m_ResCache->RegisterLoader(CreateSdkMeshResourceLoader());
    m_ResCache->RegisterLoader(CreateScriptResourceLoader());

	if(!LoadStrings("English"))
	{
        GCC_ERROR("Failed to load strings");
		return false;
	}

	//创建Lua脚本管理器单实例
    if (!LuaStateManager::Create())
	{
        GCC_ERROR("Failed to initialize Lua");
		return false;
	}

    //加载PreInit.lua文件至缓存系统中，后面可以直接调用而无需再从磁盘加载
    {
        Resource resource(SCRIPT_PREINIT_FILE);
        shared_ptr<ResHandle> pResourceHandle = m_ResCache->GetHandle(&resource);
    }

	m_Options.AddToScriptEnvironment();

    //将C++中的方法注册到Lua中，使之可以调用
    ScriptExports::Register();
	//设置Lua中的Process元表，并将控制进程的方法绑定到元表中（在lua中该表称之为SciptProcess)
    ScriptProcess::RegisterScriptClass();
	//设置Lua中的Component元表，并将常见的方法（如GetPosition)绑定到元表中(该元表称之为BaseScriptComponentMetaTable）
    BaseScriptComponent::RegisterScriptFunctions();

	//创建事件管理器
	m_pEventManager = GCC_NEW EventManager("Engine Event Mgr", true );
	if (!m_pEventManager)
	{
		GCC_ERROR("Failed to create EventManager.");
		return false;
	}

	//使用DXUT框架来初始化DX环境
	DXUTInit( true, true, lpCmdLine, true ); 
	if (hWnd==NULL)
	{
	    DXUTCreateWindow( VGetGameTitle(), hInstance, VGetIcon() );
	}
	else
	{
	    DXUTSetWindow( hWnd, hWnd, hWnd );
	}

	if (!GetHwnd())
	{
		return FALSE;
	}
	SetWindowText(GetHwnd(), VGetGameTitle());

	// initialize the directory location you can store save game files
	_tcscpy_s(m_saveGameDirectory, GetSaveGameDirectory(GetHwnd(), VGetGameAppDirectory()));

	m_screenSize = Point(screenWidth, screenHeight);

	//使用DX11 如果使DX9,参数配置为 D3D_FEATURE_LEVEL_9_3
	DXUTCreateDevice( D3D_FEATURE_LEVEL_10_1, true, screenWidth, screenHeight);
	
	if (GetRendererImpl() == Renderer_D3D9)
	{
		m_Renderer = shared_ptr<IRenderer>(GCC_NEW D3DRenderer9());
	}
	else if (GetRendererImpl() == Renderer_D3D11)
	{
		m_Renderer = shared_ptr<IRenderer>(GCC_NEW D3DRenderer11());
	}
	m_Renderer->VSetBackgroundColor(0, 0, 0, 255);
	m_Renderer->VOnRestore();

	//初始化游戏逻辑和视图
	m_pGame = VCreateGameAndView();
	if (!m_pGame)
		return false;

	//预载资源文件
	m_ResCache->Preload("*.ogg", NULL);
	m_ResCache->Preload("*.dds", NULL);
	m_ResCache->Preload("*.jpg", NULL);
	//预载网格文件
	if ( Engine::GetRendererImpl() == Engine::Renderer_D3D11 )
		m_ResCache->Preload("*.sdkmesh", NULL);

	//检查手柄
	CheckForJoystick(GetHwnd());

	m_bIsRunning = true;

	return TRUE;
}

//注册基础事件
void Engine::RegisterEngineEvents(void)
{
    REGISTER_EVENT(EvtData_Environment_Loaded);
    REGISTER_EVENT(EvtData_New_Actor);
    REGISTER_EVENT(EvtData_Move_Actor);
    REGISTER_EVENT(EvtData_Destroy_Actor);
	REGISTER_EVENT(EvtData_Request_New_Actor);
	REGISTER_EVENT(EvtData_Network_Player_Actor_Assignment);
}

//加载Strings文件夹下的对应语言的配置Xml 并写入map
bool Engine::LoadStrings(std::string language)
{
	std::string languageFile = "Strings\\";
	languageFile += language;
	languageFile += ".xml";

	TiXmlElement* pRoot = XmlResourceLoader::LoadAndReturnRootXmlElement(languageFile.c_str());
	if (!pRoot)
	{
		GCC_ERROR("Strings are missing.");
		return false;
	}

    for (TiXmlElement* pElem = pRoot->FirstChildElement(); pElem; pElem = pElem->NextSiblingElement())
    {
		const char *pKey=pElem->Attribute("id");
		const char *pText=pElem->Attribute("value");
		const char *pHotkey=pElem->Attribute("hotkey");
		if (pKey && pText) 
		{
			wchar_t wideKey[64];
			wchar_t wideText[1024];
			AnsiToWideCch(wideKey, pKey, 64);
			AnsiToWideCch(wideText, pText, 1024);
			m_textResource[std::wstring(wideKey)] = std::wstring(wideText);

			if (pHotkey)
			{
				m_hotkeys[std::wstring(wideKey)] = MapCharToKeycode(*pHotkey);
			}
		}
	}
	return true;
}

UINT Engine::MapCharToKeycode(const char pHotKey)
{
	if (pHotKey >= '0' && pHotKey <= '9')
		return 0x30 + pHotKey - '0';

	if (pHotKey >= 'A' && pHotKey <= 'Z')
		return 0x41 + pHotKey - 'A';

	GCC_ASSERT(0 && "Platform specific hotkey is not defined");
	return 0;	
}

//根据ID获取字符串
std::wstring Engine::GetString(std::wstring sID)
{
	auto localizedString = m_textResource.find(sID);
	if(localizedString == m_textResource.end())
	{
		GCC_ASSERT(0 && "String not found!");
		return L"";
	}
	return localizedString->second;
}
#pragma endregion

#pragma region GamePlay

bool Engine::VLoadGame(void)
{
	// Read the game options and see what the current game
	// needs to be - all of the game graphics are initialized by now, too...
	return m_pGame->VLoadGame(m_Options.m_Level.c_str());
}

//每帧世界更新 该世界更新完成后调用FrameRender进行渲染处理
void CALLBACK Engine::OnUpdateGame(double fTime, float fElapsedTime, void* pUserContext)
{
	//模态框 单独处理
	if (g_pApp->HasModalDialog())
	{
		return;
	}

	//退出
	if (g_pApp->m_bQuitting)
	{
		PostMessage(g_pApp->GetHwnd(), WM_CLOSE, 0, 0);
	}
	//更新事件管理器 网络 更新游戏
	if (g_pApp->m_pGame)
	{
		IEventManager::Get()->VUpdate(20); // allow event queue to process for up to 20 ms

		if (g_pApp->m_pBaseSocketManager)
			g_pApp->m_pBaseSocketManager->DoSelect(0);	// pause 0 microseconds

		g_pApp->m_pGame->VOnUpdate(float(fTime), fElapsedTime);
	}
}

//获取用户视图 当前只支持单屏幕
HumanView* Engine::GetHumanView()
{
	HumanView* pView = NULL;
	for (GameViewList::iterator i = m_pGame->m_gameViews.begin(); i != m_pGame->m_gameViews.end(); ++i)
	{
		if ((*i)->VGetType() == GameView_Human)
		{
			shared_ptr<IGameView> pIGameView(*i);
			pView = static_cast<HumanView*>(&*pIGameView);
			break;
		}
	}
	return pView;
}
#pragma endregion

#pragma region Message Handle

//消息处理
LRESULT CALLBACK Engine::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
    // Always allow dialog resource manager calls to handle global messages
    // so GUI state is updated correctly
    *pbNoFurtherProcessing = D3DRenderer::g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

	LRESULT result = 0;

	switch (uMsg) 
	{
		case WM_POWERBROADCAST:
		{
			int event = (int)wParam;
			result = g_pApp->OnPowerBroadcast(event);
			break;
		}

		case WM_DISPLAYCHANGE:
		{
			int colorDepth = (int)wParam;
			int width = (int)(short) LOWORD(lParam);
			int height = (int)(short) HIWORD(lParam);

			result = g_pApp->OnDisplayChange(colorDepth, width, height);
			break;
		}

		case WM_SYSCOMMAND: 
		{
			result = g_pApp->OnSysCommand(wParam, lParam);
			if (result)
			{
				*pbNoFurtherProcessing = true;
			}
			break;
		}

		case WM_SYSKEYDOWN:
		{
			if (wParam == VK_RETURN)
			{
				*pbNoFurtherProcessing = true;
				return g_pApp->OnAltEnter();
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}


		case WM_CLOSE:
		{
			// DXUT apps choose ESC key as a default exit command.
			// GameCode4 doesn't like this so we disable it by checking 
			// the m_bQuitting bool, and if we're not really quitting
			// set the "no further processing" parameter to true.
			if (g_pApp->m_bQuitting)
			{
				result = g_pApp->OnClose();
			}
			else
			{
				*pbNoFurtherProcessing = true;
			}
			break;
		}

		case WM_KEYDOWN:
        case WM_KEYUP:
		case WM_CHAR:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case MM_JOY1BUTTONDOWN:
		case MM_JOY1BUTTONUP:
		case MM_JOY1MOVE:
		case MM_JOY1ZMOVE:
		case MM_JOY2BUTTONDOWN:
		case MM_JOY2BUTTONUP:
		case MM_JOY2MOVE:
		case MM_JOY2ZMOVE:
		{
			//鼠标键盘手柄等输入事件一并由对应的视图处理
			if (g_pApp->m_pGame)
			{
				BaseGameLogic *pGame = g_pApp->m_pGame;
				// Note the reverse order! User input is grabbed first from the view that is on top, 
				// which is the last one in the list.
				AppMsg msg;
				msg.m_hWnd = hWnd;
				msg.m_uMsg = uMsg;
				msg.m_wParam = wParam;
				msg.m_lParam = lParam;
				for(GameViewList::reverse_iterator i=pGame->m_gameViews.rbegin(); i!=pGame->m_gameViews.rend(); ++i)
				{
					if ( (*i)->VOnMsgProc( msg ) )
					{
						result = true;
						break;				// WARNING! This breaks out of the for loop.
					}
				}
			}
			break;
		}
	}

	return result;
}

//窗口创建前的事件 如更改边框等
LRESULT Engine::OnNcCreate(LPCREATESTRUCT cs)
{
	return true;
}

//桌面分辨率更改事件
LRESULT Engine::OnDisplayChange(int colorDepth, int width, int height)
{
	m_rcDesktop.left = 0;
	m_rcDesktop.top = 0; 
	m_rcDesktop.right = width;
	m_rcDesktop.bottom = height;
	m_iColorDepth = colorDepth;
	return 0;
}

//因电源(如休眠)挂起系统的事件
LRESULT Engine::OnPowerBroadcast(int event)
{
	//阻止休眠
	if ( event == PBT_APMQUERYSUSPEND )
		return BROADCAST_QUERY_DENY;
	else if ( event == PBT_APMBATTERYLOW )
	{
		AbortGame();
	}

	return true;
}

//处理系统消息
LRESULT Engine::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case SC_MAXIMIZE :
		{
			// If windowed and ready...
			if ( m_bWindowedMode && IsRunning() )
			{
				// Make maximize into FULLSCREEN toggle
				OnAltEnter();
			}
		}
		return 0;

		case SC_CLOSE :
		{
			// The quit dialog confirmation would appear once for
			// every SC_CLOSE we get - which happens multiple times
			// if modal dialogs are up.  This now uses the g_QuitNoPrompt
			// flag to only prompt when receiving a SC_CLOSE that isn't
			// generated by us (identified by g_QuitNoPrompt).
			
			// If closing, prompt to close if this isn't a forced quit
			if ( lParam != g_QuitNoPrompt )
			{
				// ET - 05/21/01 - Bug #1916 - Begin
				// We were receiving multiple close dialogs
				// when closing again ALT-F4 while the close
				// confirmation dialog was up.
				// Eat if already servicing a close
				if ( m_bQuitRequested )
					return true;

				// Wait for the application to be restored
				// before going any further with the new 
				// screen.  Flash until the person selects
				// that they want to restore the game, then
				// reinit the display if fullscreen.  
				// The reinit is necessary otherwise the game
				// will switch to windowed mode.
	
				// Quit requested
				m_bQuitRequested = true;
				// Prompt
				if ( MessageBox::Ask(QUESTION_QUIT_GAME) == IDNO )
				{
					// Bail - quit aborted
					
					// Reset quit requested flag
					m_bQuitRequested = false;
					
					return true;
				}
			}
	
			m_bQuitting = true;

			// Is there a game modal dialog up?
			if ( HasModalDialog() )
			{
				// Close the modal
				// and keep posting close to the app
				ForceModalExit();

				// Reissue the close to the app
				
				// Issue the new close after handling the current one,
				// but send in g_QuitNoPrompt to differentiate it from a 
				// regular CLOSE issued by the system.
				PostMessage( GetHwnd(), WM_SYSCOMMAND, SC_CLOSE, g_QuitNoPrompt );

				m_bQuitRequested = false;
				
				// Eat the close
				return true;
			}

			// Reset the quit after any other dialogs have popped up from this close
			m_bQuitRequested = false;
		}
		return 0;

		default:
			// return non-zero of we didn't process the SYSCOMMAND message
			return DefWindowProc(GetHwnd(), WM_SYSCOMMAND, wParam, lParam);
	}

	return 0;
}

//程序关闭事件
LRESULT Engine::OnClose()
{
	//释放所有系统资源
	SAFE_DELETE(m_pGame);

	DestroyWindow(GetHwnd());

	VDestroyNetworkEventForwarder();

	SAFE_DELETE(m_pBaseSocketManager);

	SAFE_DELETE(m_pEventManager);

    BaseScriptComponent::UnregisterScriptFunctions();
    ScriptExports::Unregister();
    LuaStateManager::Destroy();

	SAFE_DELETE(m_ResCache);

	return 0;
}

//最小化时闪烁任务栏图标
void Engine::FlashWhileMinimized()
{
	if (!GetHwnd())
		return;

	//闪烁窗口直到非最小化
	if (IsIconic(GetHwnd()))
	{
		// Make sure the app is up when creating a new screen
		// this should be the case most of the time, but when
		// we close the app down, minimized, and a confirmation
		// dialog appears, we need to restore
		DWORD now = timeGetTime();
		DWORD then = now;
		MSG msg;

		FlashWindow(GetHwnd(), true);

		//处理事件
		while (true)
		{
			if (PeekMessage(&msg, NULL, 0, 0, 0))
			{
				if (msg.message != WM_SYSCOMMAND || msg.wParam != SC_CLOSE)
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				//已经恢复 停止循环
				if (!IsIconic(GetHwnd()))
				{
					FlashWindow(GetHwnd(), false);
					break;
				}
			}
			else
			{
				now = timeGetTime();
				DWORD timeSpan = now > then ? (now - then) : (then - now);
				if (timeSpan > 1000)
				{
					then = now;
					FlashWindow(GetHwnd(), true);
				}
			}
		}
	}
}

//切换全屏
LRESULT Engine::OnAltEnter()
{
	DXUTToggleFullScreen();
	return 0;
}

#pragma endregion

#pragma region NetWork Handle

bool Engine::AttachAsClient()
{
	ClientSocketManager* pClient = GCC_NEW ClientSocketManager(g_pApp->m_Options.m_gameHost, g_pApp->m_Options.m_listenPort);
	if (!pClient->Connect())
	{
		return false;
	}
	g_pApp->m_pBaseSocketManager = pClient;
	VCreateNetworkEventForwarder();

	return true;
}

// Any events that will be received from the server logic should be here!
void Engine::VCreateNetworkEventForwarder(void)
{
	if (m_pNetworkEventForwarder != NULL)
	{
		GCC_ERROR("Overwriting network event forwarder in TeapotWarsApp!");
		SAFE_DELETE(m_pNetworkEventForwarder);
	}

	m_pNetworkEventForwarder = GCC_NEW NetworkEventForwarder(0);

	IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->VAddListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Request_New_Actor::sk_EventType);
	pGlobalEventManager->VAddListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Environment_Loaded::sk_EventType);
	pGlobalEventManager->VAddListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_PhysCollision::sk_EventType);

}

void Engine::VDestroyNetworkEventForwarder(void)
{
	if (m_pNetworkEventForwarder)
	{
		IEventManager* pGlobalEventManager = IEventManager::Get();
		pGlobalEventManager->VRemoveListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Request_New_Actor::sk_EventType);
		pGlobalEventManager->VRemoveListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_Environment_Loaded::sk_EventType);
		pGlobalEventManager->VRemoveListener(MakeDelegate(m_pNetworkEventForwarder, &NetworkEventForwarder::ForwardEvent), EvtData_PhysCollision::sk_EventType);
		SAFE_DELETE(m_pNetworkEventForwarder);
	}
}
#pragma endregion

#pragma region Render Setting 

Engine::Renderer Engine::GetRendererImpl()
{
	if (DXUTGetDeviceSettings().ver == DXUT_D3D9_DEVICE)
		return Renderer_D3D9;
	else
		return Renderer_D3D11;
	return Renderer_Unknown;
};

bool CALLBACK Engine::ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
	if (pDeviceSettings->ver == DXUT_D3D9_DEVICE)
	{
		IDirect3D9* pD3D = DXUTGetD3D9Object();
		D3DCAPS9 Caps;
		pD3D->GetDeviceCaps(pDeviceSettings->d3d9.AdapterOrdinal, pDeviceSettings->d3d9.DeviceType, &Caps);

		// If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
		// then switch to SWVP.
		if ((Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
			Caps.VertexShaderVersion < D3DVS_VERSION(1, 1))
		{
			pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}

		// Debugging vertex shaders requires either REF or software vertex processing 
		// and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
		if (pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF)
		{
			pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
			pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
			pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}
#endif
#ifdef DEBUG_PS
		pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
	}

	// For the first device created if its a REF device, optionally display a warning dialog box
	static bool s_bFirstTime = true;
	if (s_bFirstTime)
	{
		s_bFirstTime = false;
		if ((DXUT_D3D9_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF) ||
			(DXUT_D3D11_DEVICE == pDeviceSettings->ver &&
				pDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE))
		{
			DXUTDisplaySwitchingToREFWarning(pDeviceSettings->ver);
		}

	}

	return true;
}
#pragma endregion

#pragma region DirectX 9 CallBack
//--------------------------------------------------------------------------------------
// Rejects any D3D9 devices that aren't acceptable to the app by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK Engine::IsD3D9DeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
	D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	// Skip backbuffer formats that don't support alpha blending
	IDirect3D9* pD3D = DXUTGetD3D9Object();
	if (FAILED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,
		AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
		D3DRTYPE_TEXTURE, BackBufferFormat)))
		return false;

	// No fallback defined by this app, so reject any device that 
	// doesn't support at least ps2.0
	if (pCaps->PixelShaderVersion < D3DPS_VERSION(2, 0))
		return false;

	return true;
}

//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK Engine::OnD3D9ResetDevice(IDirect3DDevice9* pd3dDevice,
	const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr;

	if (g_pApp->m_Renderer)
	{
		V_RETURN(g_pApp->m_Renderer->VOnRestore());
	}

	if (g_pApp->m_pGame)
	{
		BaseGameLogic* pGame = g_pApp->m_pGame;
		for (GameViewList::iterator i = pGame->m_gameViews.begin(); i != pGame->m_gameViews.end(); ++i)
		{
			V_RETURN((*i)->VOnRestore());
		}
	}

	return S_OK;
}

//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the 
// rendering calls for the scene, and it will also be called if the window needs to be 
// repainted. After this function has returned, the sample framework will call 
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//
// See Game Coding Complete - 3rd Edition - 
//--------------------------------------------------------------------------------------
void CALLBACK Engine::OnD3D9FrameRender(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
	BaseGameLogic* pGame = g_pApp->m_pGame;

	for (GameViewList::iterator i = pGame->m_gameViews.begin(),
		end = pGame->m_gameViews.end(); i != end; ++i)
	{
		(*i)->VOnRender(fTime, fElapsedTime);
	}

	g_pApp->m_pGame->VRenderDiagnostics();
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK Engine::OnD3D9CreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext)
{
	HRESULT hr;

	V_RETURN(D3DRenderer::g_DialogResourceManager.OnD3D9CreateDevice(pd3dDevice));

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK Engine::OnD3D9DestroyDevice(void* pUserContext)
{
	g_pApp->m_Renderer->VShutdown();
	D3DRenderer::g_DialogResourceManager.OnD3D9DestroyDevice();
	g_pApp->m_Renderer = shared_ptr<IRenderer>(NULL);
}

void CALLBACK Engine::OnD3D9LostDevice(void* pUserContext)
{
	D3DRenderer::g_DialogResourceManager.OnD3D9LostDevice();

	if (g_pApp->m_pGame)
	{
		BaseGameLogic* pGame = g_pApp->m_pGame;
		for (GameViewList::iterator i = pGame->m_gameViews.begin(); i != pGame->m_gameViews.end(); ++i)
		{
			(*i)->VOnLostDevice();
		}
	}
}

#pragma endregion

#pragma region DirectX 11 CallBack

bool CALLBACK Engine::IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo* AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo* DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	return true;
}

HRESULT CALLBACK Engine::OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr;
	ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
	V_RETURN(D3DRenderer::g_DialogResourceManager.OnD3D11CreateDevice(pd3dDevice, pd3dImmediateContext));

	return S_OK;
}

HRESULT CALLBACK Engine::OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr;

	V_RETURN(D3DRenderer::g_DialogResourceManager.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));

	if (g_pApp->m_pGame)
	{
		BaseGameLogic* pGame = g_pApp->m_pGame;
		for (GameViewList::iterator i = pGame->m_gameViews.begin(); i != pGame->m_gameViews.end(); ++i)
		{
			(*i)->VOnRestore();
		}
	}

	return S_OK;
}

void CALLBACK Engine::OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
	float fElapsedTime, void* pUserContext)
{
	BaseGameLogic* pGame = g_pApp->m_pGame;

	for (GameViewList::iterator i = pGame->m_gameViews.begin(),
		end = pGame->m_gameViews.end(); i != end; ++i)
	{
		(*i)->VOnRender(fTime, fElapsedTime);
	}

	g_pApp->m_pGame->VRenderDiagnostics();

	RenderTextStringToScreen(fElapsedTime);
}

void Engine::AddRenderToScreenText(const std::string& str, float time, const D3DXCOLOR color) {
	m_ToRenderText.push_back(make_tuple(str, time,color));
}

void Engine::RenderTextStringToScreen(float fElapsedTime) {
	if (!g_pApp) return;
	int index = 0;
	for (auto iter = g_pApp->m_ToRenderText.begin(); iter != g_pApp->m_ToRenderText.end();) {
		std::string m_RenderToScreenText = std::get<0>(*iter);
		float& leftTime = std::get<1>(*iter);
		D3DXCOLOR color= std::get<2>(*iter);
		leftTime -= fElapsedTime*1000;
		if (leftTime < 0) {
			iter= g_pApp->m_ToRenderText.erase(iter);
			continue;
		}
		index++;
		D3DRenderer::g_pTextHelper->Begin();
		const D3DXCOLOR white(1.0f, 1.0f, 1.0f, 1.0f);
		const D3DXCOLOR black(0.0f, 0.0f, 0.0f, 1.0f);
		RECT inputTextRect, outputTextRect, shadowRect;

		inputTextRect.left = 10;
		//inputTextRect.right = g_pApp->GetScreenSize().x - 10;
		inputTextRect.right = 40;
		if(m_RenderToScreenText.size()>10)
			inputTextRect.right = std::min((size_t)600,4* m_RenderToScreenText.size());
		inputTextRect.top = 20+20*index;
		inputTextRect.bottom =40+20*index;

		const int kNumWideChars = 4096;
		WCHAR wideBuffer[kNumWideChars];
		AnsiToWideCch(wideBuffer, m_RenderToScreenText.c_str(), kNumWideChars);

		D3DRenderer::g_pTextHelper->DrawTextLine(inputTextRect, DT_LEFT | DT_TOP | DT_CALCRECT, wideBuffer);

		//Draw with shadow first.
		shadowRect = inputTextRect;
		++shadowRect.left;
		++shadowRect.top;
		D3DRenderer::g_pTextHelper->SetForegroundColor(black);
		D3DRenderer::g_pTextHelper->DrawTextLine(shadowRect, DT_LEFT | DT_TOP, wideBuffer);

		//Now bright text.
		D3DRenderer::g_pTextHelper->SetForegroundColor(color);
		D3DRenderer::g_pTextHelper->DrawTextLine(inputTextRect, DT_LEFT | DT_TOP, wideBuffer);
		D3DRenderer::g_pTextHelper->End();

		iter++;
	}
}

void CALLBACK Engine::OnD3D11ReleasingSwapChain(void* pUserContext)
{
	D3DRenderer::g_DialogResourceManager.OnD3D11ReleasingSwapChain();
}

void CALLBACK Engine::OnD3D11DestroyDevice(void* pUserContext)
{
	if (g_pApp->m_Renderer)
		g_pApp->m_Renderer->VShutdown();
	D3DRenderer::g_DialogResourceManager.OnD3D11DestroyDevice();
	g_pApp->m_Renderer = shared_ptr<IRenderer>(NULL);
}

#pragma endregion

#pragma region Modal Handle
//
// class Engine::Modal						
//
int Engine::Modal(shared_ptr<IScreenElement> pModalScreen, int defaultAnswer)
{
	// If we're going to display a dialog box, we need a human view 
	// to interact with.

	// [mrmike] This bit of code was refactored post press into Engine::GetHumanView()
	/***
	HumanView *pView;
	for(GameViewList::iterator i=m_pGame->m_gameViews.begin(); i!=m_pGame->m_gameViews.end(); ++i)
	{
		if ((*i)->VGetType()==GameView_Human)
		{
			shared_ptr<IGameView> pIGameView(*i);
			pView = static_cast<HumanView *>(&*pIGameView);
			break;
		}
	}
	***/

	HumanView* pView = GetHumanView();

	if (!pView)
	{
		// Whoops! There's no human view attached.
		return defaultAnswer;
	}

	if (m_HasModalDialog & 0x10000000)
	{
		GCC_ASSERT(0 && "Too Many nested dialogs!");
		return defaultAnswer;
	}
	
	GCC_ASSERT(GetHwnd() != NULL && _T("Main Window is NULL!"));
	if ( ( GetHwnd() != NULL ) && IsIconic(GetHwnd()) )
	{
		FlashWhileMinimized();
	}

	m_HasModalDialog <<= 1;
	m_HasModalDialog |= 1;

	pView->VPushElement(pModalScreen);

	LPARAM lParam = 0;
	int result = PumpUntilMessage(g_MsgEndModal, NULL, &lParam);
	
	if (lParam != 0)
	{
		if (lParam==g_QuitNoPrompt)
			result = defaultAnswer;
		else	
			result = (int)lParam;
	}

	pView->VRemoveElement(pModalScreen);
	m_HasModalDialog >>= 1;

	return result;
}

//
// class Engine::PumpUntilMessage			 
//
int Engine::PumpUntilMessage (UINT msgEnd, WPARAM* pWParam, LPARAM* pLParam)
{
	int currentTime = timeGetTime();
	MSG msg;
	for ( ;; )
	{
		if ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if (msg.message == WM_CLOSE)
			{
				m_bQuitting = true;
				GetMessage(& msg, NULL, 0, 0);
				break;
			}
			else
			{
				// Default processing
				if (GetMessage(&msg, NULL, NULL, NULL))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				// Are we done?
				if ( msg.message == msgEnd)
					break;
			}
		}
		else
		{
			// Update the game views, but nothing else!
			// Remember this is a modal screen.
			if (m_pGame)
			{
				int timeNow = timeGetTime();
				int deltaMilliseconds = timeNow - currentTime;
				for(GameViewList::iterator i=m_pGame->m_gameViews.begin(); i!=m_pGame->m_gameViews.end(); ++i)
				{
					(*i)->VOnUpdate( deltaMilliseconds );
				}
				currentTime = timeNow;
				DXUTRender3DEnvironment();
			}
		}
	}
	if (pLParam)
		*pLParam = msg.lParam;
	if (pWParam)
		*pWParam = msg.wParam;

	return 0;
}

//This function removes all of a *SPECIFIC* type of message from the queue...
int	Engine::EatSpecificMessages( UINT msgType, optional<LPARAM> lParam, optional<WPARAM> wParam)
{
	bool done = false;

	while (!done)
	{
		MSG msg;

		if ( PeekMessage( &msg, NULL, msgType, msgType, PM_NOREMOVE ) )
		{
			bool valid = true;

			if (lParam.valid())
			{
				valid &= (*lParam==msg.lParam);
			}

			if (wParam.valid())
			{
				valid &= (*wParam==msg.wParam);
			}

			if (valid)
			{
				//Soak!
				GetMessage(& msg, NULL, msgType, msgType);
			}
			else
			{
				done = true;
			}
		}
		else
		{
			done = true;	//No more messages!
		}
	}

	return 0;
}
#pragma endregion