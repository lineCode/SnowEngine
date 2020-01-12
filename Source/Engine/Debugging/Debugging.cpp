// Debugging.cpp : Miscellaneous debugging tools

#include "EngineStd.h"

#include <unknwn.h>
int Refs(IUnknown *pUnk)
{
	pUnk->AddRef();
	return pUnk->Release();
}
