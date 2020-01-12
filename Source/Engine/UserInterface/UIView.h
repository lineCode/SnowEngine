#pragma once
// UIView.h : source file for the ui of Lua

#include <DXUTGui.h>
#include "../Scene/SceneNodes.h"
#include "../EventManager/EventManager.h"
#include "../UserInterface/UserInterface.h"
#include "../UserInterface/HumanView.h"

#include<vector>
#include<map>

class MainUI : public BaseUI
{
protected:
	// dialog for sample specific controls
	CDXUTDialog m_SampleUI;

public:
	MainUI(const char* UIScriptFileName = "scripts\\MenuUI.lua");
	virtual ~MainUI();

	// IScreenElement Implementation
	virtual HRESULT VOnRestore();
	virtual HRESULT VOnRender(double fTime, float fElapsedTime);
	virtual int VGetZOrder() const { return 1; }
	virtual void VSetZOrder(int const zOrder) { }

	virtual LRESULT CALLBACK VOnMsgProc(AppMsg msg);
	static void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);
	void CALLBACK _OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);

private:
	LuaPlus::LuaObject m_OnInitFunction;
	LuaPlus::LuaObject m_OnUpdateFunction;

	//绑定全局的创建控件函数
	void RegisterBaseFunctions();

private:
	std::vector<CDXUTControl*> m_HandleBuffer;
	std::map<int, LuaPlus::LuaObject> m_CallBackFunctions;

	//Lua调用 实现控件事件回调绑定
	void RegisterCallBack(LuaPlus::LuaObject widgetObject, LuaPlus::LuaObject callBackFunction);

	//Base FUnction Bind
	void SetBackgroundColors();
	void SetFont();

	//Common Functions
	void SetVisible(LuaPlus::LuaObject object, bool isVisible);

	//RadioButton
	LuaPlus::LuaObject AddRadioButton(const char* name, int x, int y, int width, int height);
	void SetChecked(LuaPlus::LuaObject thisListInstance, bool isChecked);

	//ListBox
	LuaPlus::LuaObject AddListBox(int x, int y, int width, int height);
	LuaPlus::LuaObject AddListItem(LuaPlus::LuaObject thisListInstance, const char* itemName);
	int GetSelectedIndex(LuaPlus::LuaObject thisListInstance);

	//Static Widget
	LuaPlus::LuaObject AddStatic(const char* name, int x, int y, int width, int height);
	void SetText(LuaPlus::LuaObject thisStaticInstance, const char* text);

	//Slider Widget
	LuaPlus::LuaObject AddSlider(const char* name, int x, int y, int width, int height);
	int GetValue(LuaPlus::LuaObject thisSliderInstance);

	//EditBox
	LuaPlus::LuaObject AddEditBox(const char* name, int x, int y, int width, int height);
	LuaPlus::LuaObject GetText(LuaPlus::LuaObject thisEditBoxInstance);

	//Button
	LuaPlus::LuaObject AddButton(const char* name, int x, int y, int width, int height);

};

class UIView : public HumanView
{
protected:
	shared_ptr<MainUI> m_MainMenuUI;
public:
	UIView();
	~UIView();
	virtual LRESULT CALLBACK VOnMsgProc(AppMsg msg);
	virtual void VRenderText();
	virtual void VOnUpdate(unsigned long deltaMs);
};