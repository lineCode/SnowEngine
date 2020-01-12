#pragma once
// Minidump.h : This is a crash trapper - similar to a UNIX style core dump

//========================================================================
// This is a simple class for handling minidumps.  Here�s an example of this class at work:
// 
// MiniDumper gMiniDumper;
// int main()
// {
//    *(int *)0 = 12;      // CRASH!!!!! 
//    return 0;
// }
// 
// Just declare a global singleton of the MiniDumper, and when an unhandled exception comes 
// along, your player will get asked if he or she wants to write out some diagnostic 
// information. The minidump file will appear in the same directory as your executable, ready 
// for debugging.
// 
// If you want to save the minidump file with a different name, inherit from the MiniDump class 
// and overload VSetDumpFileName. One thing you might consider doing is putting a timestamp in 
// the dump file name, so that one minidump file doesn�t overwrite another. If you�d like to 
// include your own data stream, overload VGetUserStreamArray(). 

#include <Tlhelp32.h>
#include "dbghelp.h"

#include <list>

class MiniDumper
{
protected:
	static MiniDumper *gpDumper;
	static LONG WINAPI Handler( struct _EXCEPTION_POINTERS *pExceptionInfo );

	_EXCEPTION_POINTERS *m_pExceptionInfo;
	time_t m_lTime;

	TCHAR m_szDumpPath[_MAX_PATH];
	TCHAR m_szAppPath[_MAX_PATH];
	TCHAR m_szAppBaseName[_MAX_PATH];
	LONG WriteMiniDump(_EXCEPTION_POINTERS *pExceptionInfo );
	BOOL m_bHeadless;

	virtual void VSetDumpFileName(void);
	virtual MINIDUMP_USER_STREAM_INFORMATION *VGetUserStreamArray() { return NULL; }
	virtual const TCHAR *VGetUserMessage() { return _T(""); }

public:
	MiniDumper(bool headless);
};

class ModuleEntry
{
protected:
	MODULEENTRY32 m_me32;
	TCHAR *m_pFileVersionInfo;
	TCHAR *m_pProductVersion;

	void ReadFileVersionInfo(void);
	void FindProductVersion(void);

public:
	ModuleEntry(const MODULEENTRY32 &me);
	const MODULEENTRY32 *Get() const { return &m_me32; }
	const TCHAR *GetProductVersion() const { return m_pProductVersion; }
	virtual ~ModuleEntry();
};

#define USER_DATA_BUFFER_SIZE (4096)

