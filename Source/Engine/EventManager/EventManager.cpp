// EventManager.cpp : implements a multi-listener multi-sender event system


#include "EngineStd.h"
#include "EventManager.h"


static IEventManager* g_pEventMgr = NULL;
GenericObjectFactory<IEventData, EventType> g_eventFactory;

//GCC_MEMORY_WATCHER_DEFINITION(IEventData);

IEventManager* IEventManager::Get(void)
{
    GCC_ASSERT(g_pEventMgr);
	return g_pEventMgr;
}

IEventManager::IEventManager(const char* pName, bool setAsGlobal)
{
	if (setAsGlobal)
    {
        if (g_pEventMgr)
        {
            GCC_ERROR("Attempting to create two global event managers! The old one will be destroyed and overwritten with this one.");
            delete g_pEventMgr;
        }

		g_pEventMgr = this;
    }
}

IEventManager::~IEventManager(void)
{
	if (g_pEventMgr == this)
		g_pEventMgr = NULL;
}

