// ---------------------------------------------------------------------------
// wxadv.h - headers and wxLua types for wxLua binding
//
// This file was generated by genwxbind.lua 
// Any changes made to this file will be lost when the file is regenerated
// ---------------------------------------------------------------------------

#ifndef __HOOK_WXLUA_wxadv_H__
#define __HOOK_WXLUA_wxadv_H__

#include "wxbind/include/wxbinddefs.h"
#include "wxluasetup.h"
#include "wxbind/include/wxcore_bind.h"

#include "wxlua/wxlstate.h"
#include "wxlua/wxlbind.h"

// ---------------------------------------------------------------------------
// Check if the version of binding generator used to create this is older than
//   the current version of the bindings.
//   See 'bindings/genwxbind.lua' and 'modules/wxlua/wxldefs.h'
#if WXLUA_BINDING_VERSION > 30
#   error "The WXLUA_BINDING_VERSION in the bindings is too old, regenerate bindings."
#endif //WXLUA_BINDING_VERSION > 30
// ---------------------------------------------------------------------------

// binding class
class WXDLLIMPEXP_BINDWXADV wxLuaBinding_wxadv : public wxLuaBinding
{
public:
    wxLuaBinding_wxadv();


private:
    DECLARE_DYNAMIC_CLASS(wxLuaBinding_wxadv)
};


// initialize wxLuaBinding_wxadv for all wxLuaStates
extern WXDLLIMPEXP_BINDWXADV wxLuaBinding* wxLuaBinding_wxadv_init();

// ---------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------

#if (wxLUA_USE_wxWave) && (defined(__WXMSW__) && !wxCHECK_VERSION(2,6,0) && wxUSE_WAVE)
    #include "wx/wave.h"
#endif // (wxLUA_USE_wxWave) && (defined(__WXMSW__) && !wxCHECK_VERSION(2,6,0) && wxUSE_WAVE)

#if (wxLUA_USE_wxWave) && (wxCHECK_VERSION(2,6,0) && wxUSE_SOUND)
    #include "wx/sound.h"
#endif // (wxLUA_USE_wxWave) && (wxCHECK_VERSION(2,6,0) && wxUSE_SOUND)

#if wxCHECK_VERSION(2,8,0) && wxLUA_USE_wxAnimation && wxUSE_ANIMATIONCTRL
    #include "wx/animate.h"
#endif // wxCHECK_VERSION(2,8,0) && wxLUA_USE_wxAnimation && wxUSE_ANIMATIONCTRL

#if wxCHECK_VERSION(2,8,0) && wxUSE_ABOUTDLG && wxLUA_USE_wxAboutDialog
    #include "wx/aboutdlg.h"
#endif // wxCHECK_VERSION(2,8,0) && wxUSE_ABOUTDLG && wxLUA_USE_wxAboutDialog

#if wxCHECK_VERSION(2,8,0) && wxUSE_HYPERLINKCTRL && wxLUA_USE_wxHyperlinkCtrl
    #include "wx/hyperlink.h"
#endif // wxCHECK_VERSION(2,8,0) && wxUSE_HYPERLINKCTRL && wxLUA_USE_wxHyperlinkCtrl

#if wxLUA_USE_wxBitmapComboBox && wxUSE_BITMAPCOMBOBOX
    #include "wx/bmpcbox.h"
#endif // wxLUA_USE_wxBitmapComboBox && wxUSE_BITMAPCOMBOBOX

#if wxLUA_USE_wxCalendarCtrl && wxUSE_CALENDARCTRL
    #include "wx/calctrl.h"
    #include "wx/dateevt.h"
    #include "wx/event.h"
#endif // wxLUA_USE_wxCalendarCtrl && wxUSE_CALENDARCTRL

#if wxLUA_USE_wxGrid && wxUSE_GRID
    #include "wx/dynarray.h"
    #include "wx/generic/gridctrl.h"
    #include "wx/grid.h"
    #include "wxbind/include/wxadv_wxladv.h"
#endif // wxLUA_USE_wxGrid && wxUSE_GRID

#if wxLUA_USE_wxJoystick && wxUSE_JOYSTICK
    #include "wx/event.h"
    #include "wx/joystick.h"
#endif // wxLUA_USE_wxJoystick && wxUSE_JOYSTICK

#if wxLUA_USE_wxSashWindow && wxUSE_SASH
    #include "wx/laywin.h"
    #include "wx/sashwin.h"
#endif // wxLUA_USE_wxSashWindow && wxUSE_SASH

#if wxLUA_USE_wxSplashScreen
    #include "wx/splash.h"
#endif // wxLUA_USE_wxSplashScreen

#if wxLUA_USE_wxTaskBarIcon && defined (wxHAS_TASK_BAR_ICON )
    #include "wx/taskbar.h"
#endif // wxLUA_USE_wxTaskBarIcon && defined (wxHAS_TASK_BAR_ICON )

#if wxUSE_WIZARDDLG && wxLUA_USE_wxWizard
    #include "wx/wizard.h"
#endif // wxUSE_WIZARDDLG && wxLUA_USE_wxWizard

// ---------------------------------------------------------------------------
// Lua Tag Method Values and Tables for each Class
// ---------------------------------------------------------------------------

#if (wxLUA_USE_wxWave) && (defined(__WXMSW__) && !wxCHECK_VERSION(2,6,0) && wxUSE_WAVE)
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxWave;
#endif // (wxLUA_USE_wxWave) && (defined(__WXMSW__) && !wxCHECK_VERSION(2,6,0) && wxUSE_WAVE)

#if (wxLUA_USE_wxWave) && (wxCHECK_VERSION(2,6,0) && wxUSE_SOUND)
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxSound;
#endif // (wxLUA_USE_wxWave) && (wxCHECK_VERSION(2,6,0) && wxUSE_SOUND)

#if wxCHECK_VERSION(2,8,0) && wxLUA_USE_wxAnimation && wxUSE_ANIMATIONCTRL
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxAnimation;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxAnimationCtrl;
#endif // wxCHECK_VERSION(2,8,0) && wxLUA_USE_wxAnimation && wxUSE_ANIMATIONCTRL

#if wxCHECK_VERSION(2,8,0) && wxUSE_ABOUTDLG && wxLUA_USE_wxAboutDialog
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxAboutDialogInfo;
#endif // wxCHECK_VERSION(2,8,0) && wxUSE_ABOUTDLG && wxLUA_USE_wxAboutDialog

#if wxCHECK_VERSION(2,8,0) && wxUSE_HYPERLINKCTRL && wxLUA_USE_wxHyperlinkCtrl
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxHyperlinkCtrl;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxHyperlinkEvent;
#endif // wxCHECK_VERSION(2,8,0) && wxUSE_HYPERLINKCTRL && wxLUA_USE_wxHyperlinkCtrl

#if wxLUA_USE_wxBitmapComboBox && wxUSE_BITMAPCOMBOBOX
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxBitmapComboBox;
#endif // wxLUA_USE_wxBitmapComboBox && wxUSE_BITMAPCOMBOBOX

#if wxLUA_USE_wxCalendarCtrl && wxUSE_CALENDARCTRL
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxCalendarCtrl;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxCalendarDateAttr;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxCalendarEvent;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxDateEvent;
#endif // wxLUA_USE_wxCalendarCtrl && wxUSE_CALENDARCTRL

#if wxLUA_USE_wxGrid && wxUSE_GRID
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGrid;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellAttr;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellAttrProvider;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellAutoWrapStringEditor;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellAutoWrapStringRenderer;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellBoolEditor;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellBoolRenderer;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellChoiceEditor;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellCoords;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellCoordsArray;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellDateTimeRenderer;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellEditor;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellEnumEditor;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellEnumRenderer;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellFloatEditor;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellFloatRenderer;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellNumberEditor;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellNumberRenderer;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellRenderer;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellStringRenderer;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellTextEditor;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridCellWorker;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridEditorCreatedEvent;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridEvent;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridRangeSelectEvent;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridSizeEvent;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridStringTable;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridTableBase;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxGridTableMessage;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxLuaGridTableBase;
#endif // wxLUA_USE_wxGrid && wxUSE_GRID

#if wxLUA_USE_wxJoystick && wxUSE_JOYSTICK
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxJoystick;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxJoystickEvent;
#endif // wxLUA_USE_wxJoystick && wxUSE_JOYSTICK

#if wxLUA_USE_wxSashWindow && wxUSE_SASH
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxCalculateLayoutEvent;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxLayoutAlgorithm;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxQueryLayoutInfoEvent;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxSashEvent;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxSashLayoutWindow;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxSashWindow;
#endif // wxLUA_USE_wxSashWindow && wxUSE_SASH

#if wxLUA_USE_wxSplashScreen
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxSplashScreen;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxSplashScreenWindow;
#endif // wxLUA_USE_wxSplashScreen

#if wxLUA_USE_wxTaskBarIcon && defined (wxHAS_TASK_BAR_ICON )
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxTaskBarIcon;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxTaskBarIconEvent;
#endif // wxLUA_USE_wxTaskBarIcon && defined (wxHAS_TASK_BAR_ICON )

#if wxUSE_WIZARDDLG && wxLUA_USE_wxWizard
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxWizard;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxWizardEvent;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxWizardPage;
    extern WXDLLIMPEXP_DATA_BINDWXADV(int) wxluatype_wxWizardPageSimple;
#endif // wxUSE_WIZARDDLG && wxLUA_USE_wxWizard



#endif // __HOOK_WXLUA_wxadv_H__

