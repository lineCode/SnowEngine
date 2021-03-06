#pragma once
//========================================================================
// GameCodeStd.h : include file for standard system include files,
//            or project specific include files that are used frequently, but
//            are changed infrequently

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:

#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

#include <crtdbg.h>

#define _VS2019_
#if _MSC_VER < 1920
	#undef _VS2005_
#endif

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <mmsystem.h>

#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <queue>
#include <map>

//  Smart pointers were added to the std namespace in Visual Studio 2012.  They exist in 2010 in the 
// std::tr1 namespace.
#if _MSC_VER >= 1700  // VS 2012 or later
    #include <memory>
    using std::shared_ptr;
    using std::weak_ptr;
    using std::static_pointer_cast;
    using std::dynamic_pointer_cast;
#elif _MSC_VER == 1600  // VS 2010
    using std::tr1::shared_ptr;
    using std::tr1::weak_ptr;
    using std::tr1::static_pointer_cast;
    using std::tr1::dynamic_pointer_cast;
#endif

class GCC_noncopyable 
{  
     private:  
      GCC_noncopyable(const GCC_noncopyable& x);  
      GCC_noncopyable& operator=(const GCC_noncopyable& x);  
     public:  
      GCC_noncopyable() {}; // Default constructor  
};  


#if defined(_DEBUG)
#	define GCC_NEW new(_NORMAL_BLOCK,__FILE__, __LINE__)
#else
#	define GCC_NEW new
#endif

#define DXUT_AUTOLIB

// DirectX Includes
#include <dxut.h>
#include <d3dx9tex.h>
#include <dxut.h>
#include <SDKmisc.h>

#include <tinyxml.h>

// fast delegate stuff
#include "../3rdParty/FastDelegate/FastDelegate.h"
using fastdelegate::MakeDelegate;

#pragma warning( disable : 4996 ) // 'function' declared deprecated - gets rid of all those 2005 warnings....


// GameCode #includes
#include "..\Debugging\Logger.h"  // this should be the first of the gcc includes since it defines GCC_ASSERT()
#include "..\Utilities\types.h"
#include "..\Utilities\templates.h"
#include "..\Graphics3D\geometry.h"

typedef D3DXCOLOR Color;

extern Color g_White;
extern Color g_Black;
extern Color g_Cyan;
extern Color g_Red;
extern Color g_Green;
extern Color g_Blue;
extern Color g_Yellow;
extern Color g_Gray40;
extern Color g_Gray25;
extern Color g_Gray65;
extern Color g_Transparent;

extern Vec3 g_Up;
extern Vec3 g_Right;
extern Vec3 g_Forward;

extern Vec4 g_Up4;
extern Vec4 g_Right4;
extern Vec4 g_Forward4;



//  AppMsg				

struct AppMsg
{
	HWND m_hWnd;
	UINT m_uMsg;
	WPARAM m_wParam;
	LPARAM m_lParam;
};



#include "..\Engine\interfaces.h"


// Useful #defines

extern const float fOPAQUE;
extern const int iOPAQUE;
extern const float fTRANSPARENT;
extern const int iTRANSPARENT;

extern const int MEGABYTE;
extern const float SIXTY_HERTZ;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

#if !defined(SAFE_DELETE)
	#define SAFE_DELETE(x) if(x) delete x; x=NULL;
#endif

#if !defined(SAFE_DELETE_ARRAY)
	#define SAFE_DELETE_ARRAY(x) if (x) delete [] x; x=NULL; 
#endif

#if !defined(SAFE_RELEASE)
	#define SAFE_RELEASE(x) if(x) x->Release(); x=NULL;
#endif

#ifdef UNICODE
	#define _tcssprintf wsprintf
	#define tcsplitpath _wsplitpath
#else
	#define _tcssprintf sprintf
	#define tcsplitpath _splitpath
#endif

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning Msg: "

#include "..\Engine\Engine.h"

extern INT WINAPI Entrance(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPWSTR    lpCmdLine,
                     int       nCmdShow);

