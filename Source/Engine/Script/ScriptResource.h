#pragma once
// ScriptResource.h
// Purpose: The declaration of a quick'n dirty ZIP file reader class.Original code from Javier Arevalo.
//          Get zlib from http://www.cdrom.com/pub/infozip/zlib/

#include "../ResourceCache/ResCache.h"
#include"../Engine/Engine.h"

class ScriptResourceLoader : public IResourceLoader
{
public:
    virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() {
		if (g_pApp && g_pApp->m_pGame)
			return g_pApp->m_pGame->isRunLua; 
		return true;
	}
    virtual bool VAddNullZero() { return true; }
    virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize) { return rawSize; }
    virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
    virtual std::string VGetPattern() { return "*.lua"; }
};


