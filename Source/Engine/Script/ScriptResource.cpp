// XmlResource.cpp : Defines a simple resource cache

#include "EngineStd.h"
#include "ScriptResource.h"
#include "LuaStateManager.h"

bool ScriptResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
    if (rawSize <= 0)
        return false;
	handle->m_buffer = rawBuffer;

	if (!g_pApp->m_pGame || g_pApp->m_pGame->CanRunLua())
		LuaStateManager::Get()->VExecuteString(rawBuffer);

    return true;
}

shared_ptr<IResourceLoader> CreateScriptResourceLoader()
{
    return shared_ptr<IResourceLoader>(GCC_NEW ScriptResourceLoader());
}