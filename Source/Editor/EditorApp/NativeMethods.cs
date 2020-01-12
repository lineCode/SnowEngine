// NativeMethods.cs - exposes the C++ DLL functions

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

//========================================================================
//
// This file sets up some of the groundwork enabling us to call functions
// in our unmanaged dll from C# code
//
//========================================================================

namespace EditorApp
{
    //
    // class NativeMethods                  - 
    //
    // We also need to import some functions that will allow us to load
    // the dll, and free it when we're done.
    static class NativeMethods
    {
        const string editorDllName = "GCC4EditorDLL.dll";

        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern int EditorMain(IntPtr instancePtrAddress, IntPtr hPrevInstancePtrAddress, IntPtr hWndPtrAddress, int nCmdShow, int screenWidth, int screenHeight);
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RenderFrame();
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Shutdown();
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void WndProc(IntPtr hWndPtrAddress, int msg, int wParam, int lParam);

        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SaveLevel(string lFileName);
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void OpenLevel([MarshalAs(UnmanagedType.BStr)] string lFileName);

        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetNumActors();
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void GetActorList(IntPtr actorIdArrayPtrAddress, int size);
        
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern int PickActor(IntPtr hWndPtrAddress);
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern int GetActorXmlSize(uint actorId);
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void GetActorXml(IntPtr actorXMLPtrAddress, uint actorId);

        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int CreateActor([MarshalAs(UnmanagedType.BStr)] string lactorResource);
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ModifyActor([MarshalAs(UnmanagedType.BStr)] string lactorModXML);
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern int DestroyActor(uint actorId);

        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern int GetLevelScriptAdditionsXmlSize();
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void GetLevelScriptAdditionsXml(IntPtr actorXMLPtrAddress);    
    }
}
