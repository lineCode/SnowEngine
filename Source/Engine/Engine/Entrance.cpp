//Engine Enterance Point

#include "EngineStd.h"
#include "Engine.h"
#include "../Audio/Audio.h"
#include "../Debugging/Minidump.h"

#pragma comment(lib, "shell32.lib")

// useful for Windows Registry queries
#pragma comment(lib, "advapi32.lib")			

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
		#pragma comment(lib, "BulletCollision_x64_debug.lib")
		#pragma comment(lib, "BulletDynamics_x64_debug.lib")
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
#pragma comment(lib, "assimp-vc142-mtd.lib")

MiniDumper g_MiniDump(false);				

//Engine���,ʹ��DXUT���
INT WINAPI Entrance(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPWSTR    lpCmdLine,
                     int       nCmdShow)
{
	// ����ڴ�й© ��Ŀ����->C++->�������� �����ö��̵߳��ԣ�/MTd)
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	//���ø÷��Ž���ʹ�ͷŵ��ڴ�һֱ�ڶ������� ֵΪ0xDD ���ܻᵼ���ڴ�ľ�
	// tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;	  

	//���ø÷��Ż���ÿ�������ͷ�����ʱ����й©��飬����������ϵͳ�ٶ�
	//tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;		   

	//���ø÷��Ż��ڳ����˳�ʱ�����ڴ�й©��Ⲣ���ɱ���
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;			

	_CrtSetDbgFlag(tmpDbgFlag);

   //��ʼ����־ϵͳ
    Logger::Init("logging.xml");

	g_pApp->m_Options.Init("PlayerOptions.xml", lpCmdLine);
    
	//���ûص� ��Ϣ�� ������� �豸���Ĵ���
	DXUTSetCallbackMsgProc( Engine::MsgProc );
	DXUTSetCallbackFrameMove( Engine::OnUpdateGame );
    DXUTSetCallbackDeviceChanging( Engine::ModifyDeviceSettings );

	if (g_pApp->m_Options.m_Renderer == "Direct3D 9")
	{
		DXUTSetCallbackD3D9DeviceAcceptable( Engine::IsD3D9DeviceAcceptable );
		DXUTSetCallbackD3D9DeviceCreated( Engine::OnD3D9CreateDevice );
		DXUTSetCallbackD3D9DeviceReset( Engine::OnD3D9ResetDevice );
		DXUTSetCallbackD3D9DeviceLost( Engine::OnD3D9LostDevice );
		DXUTSetCallbackD3D9DeviceDestroyed( Engine::OnD3D9DestroyDevice );
		DXUTSetCallbackD3D9FrameRender( Engine::OnD3D9FrameRender );
	}
	else if (g_pApp->m_Options.m_Renderer == "Direct3D 11")
	{
		DXUTSetCallbackD3D11DeviceAcceptable( Engine::IsD3D11DeviceAcceptable );
		DXUTSetCallbackD3D11DeviceCreated( Engine::OnD3D11CreateDevice );
		DXUTSetCallbackD3D11SwapChainResized( Engine::OnD3D11ResizedSwapChain );
		DXUTSetCallbackD3D11SwapChainReleasing( Engine::OnD3D11ReleasingSwapChain );
		DXUTSetCallbackD3D11DeviceDestroyed( Engine::OnD3D11DestroyDevice );
		DXUTSetCallbackD3D11FrameRender( Engine::OnD3D11FrameRender );
	}
	else
	{
		GCC_ASSERT(0 && "Unknown renderer specified in game options.");
		return false;
	}

    //ȫ��ʱ��ʾ��겢�ü�
    DXUTSetCursorSettings( true, true );

	//�����ʼ��
	if (!g_pApp->InitInstance (hInstance, lpCmdLine, 0, g_pApp->m_Options.m_ScreenSize.x, g_pApp->m_Options.m_ScreenSize.y)) 
	{
		return FALSE;
	}

    //�ɿ�ܴ�����Ϣ����,������£���Ⱦ����
	DXUTMainLoop();
	DXUTShutdown();

    //������־ϵͳ
    Logger::Destroy();

    return g_pApp->GetExitCode();	
}
