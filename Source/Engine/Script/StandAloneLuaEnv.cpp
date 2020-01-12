#include"../Msvc/EngineStd.h"
#include"StandAloneLuaEnv.h"

StandAloneLuaEnv::StandAloneLuaEnv()
{
	pLuaState = LuaPlus::LuaState::Create(true);
	pLuaState->GetGlobals().RegisterDirect("ExecuteFile", *this, &StandAloneLuaEnv::ExecuteFile);
	pLuaState->GetGlobals().RegisterDirect("ExecuteString", *this, &StandAloneLuaEnv::ExecuteString);
	pLuaState->GetGlobals().RegisterDirect("LoadAndExecuteScriptResource", *this, &StandAloneLuaEnv::LoadAndExecuteScriptResource);
	const char* initenvstr =
		"do"
		"local OldRequire = require;\n"
		"local resourceIdMap = {};\n"
		"require = function(script)\n"
		"if (not resourceIdMap[script]) then\n"
		"if (LoadAndExecuteScriptResource(script)) then\n"
		"resourceIdMap[script] = true;\n"
		"else\n"
		"OldRequire(script);\n"
		"end\n"
		"end\n"
		"end \n"
		"end";

	ExecuteString(initenvstr);
}

StandAloneLuaEnv::~StandAloneLuaEnv()
{
	LuaPlus::LuaState::Destroy(pLuaState);
	pLuaState = nullptr;
}

void StandAloneLuaEnv::ExecuteFile(const char* path)
{
	int result = pLuaState->DoFile(path);
	if (result != 0)
		SetError(result);
}

void StandAloneLuaEnv::ExecuteString(const char* chunk)
{
	int result = 0;

	// Most strings are passed straight through to the Lua interpreter
	if (strlen(chunk) <= 1 || chunk[0] != '=')
	{
		result = pLuaState->DoString(chunk);
		if (result != 0)
			SetError(result);
	}

	// If the string starts with '=', wrap the statement in the print() function.
	else
	{
		std::string buffer("print(");
		buffer += (chunk + 1);
		buffer += ")";
		result = pLuaState->DoString(buffer.c_str());
		if (result != 0)
			SetError(result);
	}
}

void StandAloneLuaEnv::SetError(int errorNum)
{
	LuaPlus::LuaStackObject stackObj(pLuaState, -1);
	const char* errStr = stackObj.GetString();
	if (errStr)
	{
		lastError = errStr;
		pLuaState->SetTop(0);
	}
	else
		lastError = "Unknown Lua parse error";

	GCC_ERROR(lastError);
}

shared_ptr<ResHandle> StandAloneLuaEnv::LoadScriptResource(const char* scriptResource)
{
	Resource resource(scriptResource);
	g_pApp->m_pGame->isRunLua = false;
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);
	g_pApp->m_pGame->isRunLua = true;
	if (pResourceHandle)
		return pResourceHandle;
	return shared_ptr<ResHandle>();
}

bool StandAloneLuaEnv::LoadAndExecuteScriptResource(const char* scriptResource)
{
	if (!g_pApp->m_ResCache->IsUsingDevelopmentDirectories())
	{
		Resource resource(scriptResource);
		g_pApp->m_pGame->isRunLua = false;
		shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);
		g_pApp->m_pGame->isRunLua = true;
		pLuaState->DoString(pResourceHandle->Buffer());
		if (pResourceHandle)
			return true;
		return false;
	}
	else
	{
		// If we're using development directories, have Lua execute the file directly instead of going through 
		// the resource cache.  This allows Decoda to see the file for debugging purposes.
		std::string path("..\\Assets\\");
		path += scriptResource;
		ExecuteFile(path.c_str());
		return true;
	}
}

LuaPlus::LuaState* StandAloneLuaEnv::GetState()
{
	return pLuaState;
}
