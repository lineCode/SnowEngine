#pragma once
#include"../Engine/Reflection.h"
#include"../Script/LuaStateManager.h"

// Initialization.h : Defines utility functions for game initialization

extern bool CheckStorage(const DWORDLONG diskSpaceNeeded);
extern DWORD ReadCPUSpeed();
extern bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded);
extern bool IsOnlyInstance(LPCTSTR gameTitle);
extern const TCHAR* GetSaveGameDirectory(HWND hWnd, const TCHAR* gameAppDirectory);
extern bool CheckForJoystick(HWND hWnd);

struct GameOptions
{
	REFLECT_DECLARE();
	//Game Defination
	PROPERTY_DECLARE(GameOptions, std::string, m_GameName);

	// Level option
	PROPERTY_DECLARE(GameOptions, std::string, m_Level);

	// Rendering options
	PROPERTY_DECLARE(GameOptions, std::string, m_Renderer);
	PROPERTY_DECLARE(GameOptions, bool, m_runFullSpeed);
	PROPERTY_DECLARE(GameOptions, Point, m_ScreenSize);

	// Sound options
	PROPERTY_DECLARE(GameOptions, float, m_soundEffectsVolume);
	PROPERTY_DECLARE(GameOptions, float, m_musicVolume);

	// Multiplayer options
	PROPERTY_DECLARE(GameOptions, int, m_expectedPlayers);
	PROPERTY_DECLARE(GameOptions, int, m_listenPort);
	PROPERTY_DECLARE(GameOptions, std::string, m_gameHost);
	PROPERTY_DECLARE(GameOptions, int, m_numAIs);
	PROPERTY_DECLARE(GameOptions, int, m_maxAIs);
	PROPERTY_DECLARE(GameOptions, int, m_maxPlayers);

	// resource cache options
	PROPERTY_DECLARE(GameOptions, bool, m_useDevelopmentDirectories);

	// TiXmlElement - look at this to find other options added by the developer
	TiXmlDocument* m_pDoc;

	GameOptions();
	~GameOptions() { SAFE_DELETE(m_pDoc); }

	void Init(const char* xmlFilePath, LPWSTR lpCmdLine);
	//利用反射实现脚本对游戏选项数据的自动读取和更改
	void AddToScriptEnvironment();

	LuaPlus::LuaObject ScriptGetGameOptions(LuaPlus::LuaObject thisTable,const char* key);
	void ScriptSetGameOptions(LuaPlus::LuaObject thisTable, const char* key,LuaPlus::LuaObject value);
};
