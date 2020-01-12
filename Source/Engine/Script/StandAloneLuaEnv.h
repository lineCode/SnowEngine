#pragma once
#include "LuaPlus.h"
#include<string>

class StandAloneLuaEnv {
public:
	StandAloneLuaEnv();
	~StandAloneLuaEnv();

	shared_ptr<ResHandle>  LoadScriptResource(const char* scriptResource);
	bool LoadAndExecuteScriptResource(const char* scriptResource);
	void ExecuteString(const char* chunk);

	template <typename Callee, typename Func>
	void RegisterFunction(const char* funcName, const Callee& callee, Func func);

	LuaPlus::LuaState* GetState();

private:
	LuaPlus::LuaState* pLuaState;
private:
	std::string lastError;
	void SetError(int errorNum);
	void ExecuteFile(const char* path);
	void ExecuteString(const char* chunk);
};

template <typename Callee, typename Func>
void StandAloneLuaEnv::RegisterFunction(const char* funcName, const Callee& callee, Func func)
{
	pLuaState->GetGlobals().RegisterDirect(funcName,callee,func);
}
