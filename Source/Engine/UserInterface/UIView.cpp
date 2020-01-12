// UIView.cpp : Implements of class MainUI and UIView

#include "EngineStd.h"
#include "UIView.h"
#include "../Graphics3D/D3DRenderer.h"
#include"../Shader/ShaderComplier.h"

const int g_SampleUIWidth = 600;
const int g_SampleUIHeight = 600;

MainUI::MainUI(const char* UIScriptFileName)
{
	// This line was added to comply with DirectX 9c (June 2005) 
	m_SampleUI.Init(&D3DRenderer::g_DialogResourceManager);
	m_SampleUI.SetCallback(OnGUIEvent, this);

	m_SampleUI.SetBackgroundColors(D3DCOLOR(0x00000000));
	m_SampleUI.SetFont(0, L"Ariel", 25, 0);

	RegisterBaseFunctions();

	Resource resource(UIScriptFileName);
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);

	LuaPlus::LuaObject scriptUIObject = LuaStateManager::Get()->GetGlobalVars().GetByName("menuUI");
	scriptUIObject.SetLightUserdata("__object", this);

	if (scriptUIObject.IsTable()) {
		LuaPlus::LuaObject temp = scriptUIObject.GetByName("OnInit");
		if (temp.IsFunction()) {
			m_OnInitFunction = temp;
			LuaPlus::LuaFunction<void*> func(m_OnInitFunction);
			func(scriptUIObject);
		}
		temp = scriptUIObject.GetByName("OnUpdate");
		if (temp.IsFunction()) {
			m_OnUpdateFunction = temp;
		}
	}
	ShaderComplier shader("Shader\\test.lua");
	shader.PreProcess();
	shader.Compile();
	int i = 0;
}

MainUI::~MainUI()
{
	D3DRenderer::g_DialogResourceManager.UnregisterDialog(&m_SampleUI);
}

HRESULT MainUI::VOnRestore()
{
	m_SampleUI.SetLocation((g_pApp->GetScreenSize().x - g_SampleUIWidth) / 2, (g_pApp->GetScreenSize().y - g_SampleUIHeight) / 2);
	m_SampleUI.SetSize(g_SampleUIWidth, g_SampleUIHeight);
	return S_OK;
}

HRESULT MainUI::VOnRender(double fTime, float fElapsedTime)
{
	HRESULT hr;
	DXUT_BeginPerfEvent(DXUT_PERFEVENTCOLOR, L"TeapotWarsHUD"); // These events are to help PIX identify what the code is doing
	V(m_SampleUI.OnRender(fElapsedTime));
	DXUT_EndPerfEvent();
	return S_OK;
};

LRESULT CALLBACK MainUI::VOnMsgProc(AppMsg msg)
{
	return m_SampleUI.MsgProc(msg.m_hWnd, msg.m_uMsg, msg.m_wParam, msg.m_lParam);
}

void CALLBACK MainUI::OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
	MainUI* ui = static_cast<MainUI*>(pUserContext);
	ui->_OnGUIEvent(nEvent, nControlID, pControl, pUserContext);
}

void CALLBACK MainUI::_OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
	auto iter = m_CallBackFunctions.find(nControlID);
	if (iter != m_CallBackFunctions.end()) {
		auto callBackFunction = iter->second;
		if (callBackFunction.IsFunction()) {
			LuaPlus::LuaFunction<void*> func(callBackFunction);
			func();
		}
	}

	if (m_OnUpdateFunction.IsFunction()) {
		LuaPlus::LuaObject scriptUIObject = LuaStateManager::Get()->GetGlobalVars().GetByName("menuUI");
		LuaPlus::LuaFunction<void*> func(m_OnUpdateFunction);
		func(scriptUIObject);
	}
}

void MainUI::RegisterBaseFunctions() {
	LuaPlus::LuaObject metaTableObj = LuaStateManager::Get()->GetGlobalVars().CreateTable("MainUI");
	metaTableObj.SetObject("__index", metaTableObj);
	metaTableObj.RegisterObjectDirect("AddRadioButton", (MainUI*)(0), &MainUI::AddRadioButton);
	metaTableObj.RegisterObjectDirect("AddListBox", (MainUI*)(0), &MainUI::AddListBox);
	metaTableObj.RegisterObjectDirect("AddStatic", (MainUI*)(0), &MainUI::AddStatic);
}

void MainUI::RegisterCallBack(LuaPlus::LuaObject widgetObject, LuaPlus::LuaObject callBackFunction) {
	if (!callBackFunction.IsFunction())
		return;
	if (widgetObject.IsTable()) {
		LuaPlus::LuaObject handleIndex = widgetObject.GetByName("handle");
		if (handleIndex.IsInteger()) {
			int temp = handleIndex.GetInteger();
			m_CallBackFunctions[temp] = callBackFunction;
		}
	}
}

void MainUI::SetVisible(LuaPlus::LuaObject object, bool isVisible) {
	if (object.IsTable()) {
		LuaPlus::LuaObject handleIndex = object.GetByName("handle");
		if (handleIndex.IsInteger()) {
			int temp = handleIndex.GetInteger();
			CDXUTControl* pointer = m_HandleBuffer[temp];
			pointer->SetVisible(isVisible);
		}
	}
}

LuaPlus::LuaObject MainUI::AddListBox(int x, int y, int width, int height) {
	int buffSize = m_HandleBuffer.size();

	m_SampleUI.AddListBox(buffSize, x, y, width, height);
	CDXUTControl* widget = m_SampleUI.GetListBox(buffSize);
	m_HandleBuffer.push_back(widget);

	LuaPlus::LuaObject listBox;
	listBox.AssignNewTable(LuaStateManager::Get()->GetLuaState());
	listBox.RegisterDirect("RegisterCallBack", *this, &MainUI::RegisterCallBack);
	listBox.RegisterDirect("SetVisible", *this, &MainUI::SetVisible);
	listBox.RegisterDirect("AddListItem", *this, &MainUI::AddListItem);
	listBox.RegisterDirect("GetSelectedIndex", *this, &MainUI::GetSelectedIndex);

	listBox.SetInteger("handle", buffSize);

	return listBox;
}

LuaPlus::LuaObject MainUI::AddListItem(LuaPlus::LuaObject thisListInstance, const char* itemName)
{
	if (thisListInstance.IsTable()) {
		LuaPlus::LuaObject handleIndex = thisListInstance.GetByName("handle");
		if (handleIndex.IsInteger()) {
			int temp = handleIndex.GetInteger();
			CDXUTControl* pointer = m_HandleBuffer[temp];
			if (auto listPointer = dynamic_cast<CDXUTListBox*>(pointer)) {
				std::wstring itemNameW(s2ws(itemName));
				listPointer->AddItem(itemNameW.c_str(), 0);
			}
		}
	}
	return thisListInstance;
}

int MainUI::GetSelectedIndex(LuaPlus::LuaObject thisListInstance)
{
	if (thisListInstance.IsTable()) {
		LuaPlus::LuaObject handleIndex = thisListInstance.GetByName("handle");
		if (handleIndex.IsInteger()) {
			int temp = handleIndex.GetInteger();
			CDXUTControl* pointer = m_HandleBuffer[temp];
			if (auto listPointer = dynamic_cast<CDXUTListBox*>(pointer)) {
				return listPointer->GetSelectedIndex();
			}
		}
	}
	return 0;
}

LuaPlus::LuaObject MainUI::AddRadioButton(const char* name, int x, int y, int width, int height)
{
	int buffSize = m_HandleBuffer.size();
	std::wstring nameW(s2ws(name));
	m_SampleUI.AddRadioButton(buffSize, 1, nameW.c_str(), x, y, width, height);
	CDXUTControl* widget = m_SampleUI.GetRadioButton(buffSize);
	m_HandleBuffer.push_back(widget);

	LuaPlus::LuaObject radioButton;
	radioButton.AssignNewTable(LuaStateManager::Get()->GetLuaState());
	radioButton.RegisterDirect("RegisterCallBack", *this, &MainUI::RegisterCallBack);
	radioButton.RegisterDirect("SetVisible", *this, &MainUI::SetVisible);
	radioButton.RegisterDirect("SetChecked", *this, &MainUI::SetChecked);

	radioButton.SetInteger("handle", buffSize);

	return radioButton;
}

void MainUI::SetChecked(LuaPlus::LuaObject thisRadioButtonInstance, bool isChecked)
{
	if (thisRadioButtonInstance.IsTable()) {
		LuaPlus::LuaObject handleIndex = thisRadioButtonInstance.GetByName("handle");
		if (handleIndex.IsInteger()) {
			int temp = handleIndex.GetInteger();
			CDXUTControl* pointer = m_HandleBuffer[temp];
			if (CDXUTRadioButton* buttonPointer = dynamic_cast<CDXUTRadioButton*>(pointer)) {
				buttonPointer->SetChecked(isChecked);
				return;
			}
		}
	}
}

LuaPlus::LuaObject MainUI::AddStatic(const char* name, int x, int y, int width, int height)
{
	int buffSize = m_HandleBuffer.size();
	std::wstring staticNameW(s2ws(name));
	m_SampleUI.AddStatic(buffSize, staticNameW.c_str(), x, y, width, height);
	CDXUTControl* widget = m_SampleUI.GetStatic(buffSize);
	m_HandleBuffer.push_back(widget);

	LuaPlus::LuaObject staticWidget;
	staticWidget.AssignNewTable(LuaStateManager::Get()->GetLuaState());

	staticWidget.RegisterDirect("RegisterCallBack", *this, &MainUI::RegisterCallBack);
	staticWidget.RegisterDirect("SetVisible", *this, &MainUI::SetVisible);
	staticWidget.RegisterDirect("SetText", *this, &MainUI::SetText);
	staticWidget.SetInteger("handle", buffSize);

	return staticWidget;
}

void MainUI::SetText(LuaPlus::LuaObject thisStaticInstance, const char* text)
{
	if (thisStaticInstance.IsTable()) {
		LuaPlus::LuaObject handleIndex = thisStaticInstance.GetByName("handle");
		if (handleIndex.IsInteger()) {
			int temp = handleIndex.GetInteger();
			CDXUTControl* pointer = m_HandleBuffer[temp];
			if (CDXUTStatic* staticPointer = dynamic_cast<CDXUTStatic*>(pointer)) {
				std::wstring textW(s2ws(text));
				staticPointer->SetText(textW.c_str());
			}
		}
	}
}

LuaPlus::LuaObject MainUI::AddSlider(const char* name, int x, int y, int width, int height)
{
	int buffSize = m_HandleBuffer.size();
	m_SampleUI.AddSlider(buffSize, x, y, width, height);
	CDXUTControl* widget = m_SampleUI.GetSlider(buffSize);
	m_HandleBuffer.push_back(widget);
	LuaPlus::LuaObject staticWidget;
	staticWidget.AssignNewTable(LuaStateManager::Get()->GetLuaState());

	staticWidget.RegisterDirect("RegisterCallBack", *this, &MainUI::RegisterCallBack);
	staticWidget.RegisterDirect("SetVisible", *this, &MainUI::SetVisible);
	staticWidget.RegisterDirect("GetValue", *this, &MainUI::GetValue);
	staticWidget.SetInteger("handle", buffSize);

	return staticWidget;
}

int MainUI::GetValue(LuaPlus::LuaObject thisSliderInstance)
{
	if (thisSliderInstance.IsTable()) {
		LuaPlus::LuaObject handleIndex = thisSliderInstance.GetByName("handle");
		if (handleIndex.IsInteger()) {
			int temp = handleIndex.GetInteger();
			CDXUTControl* pointer = m_HandleBuffer[temp];
			if (CDXUTSlider* sliderPointer = dynamic_cast<CDXUTSlider*>(pointer)) {
				return sliderPointer->GetValue();
			}
		}
	}
	return 0;
}

LuaPlus::LuaObject MainUI::AddEditBox(const char* name, int x, int y, int width, int height)
{
	int buffSize = m_HandleBuffer.size();
	std::wstring staticNameW(s2ws(name));
	m_SampleUI.AddEditBox(buffSize, staticNameW.c_str(), x, y, width, height);
	CDXUTControl* widget = m_SampleUI.GetEditBox(buffSize);
	m_HandleBuffer.push_back(widget);

	LuaPlus::LuaObject staticWidget;
	staticWidget.AssignNewTable(LuaStateManager::Get()->GetLuaState());

	staticWidget.RegisterDirect("RegisterCallBack", *this, &MainUI::RegisterCallBack);
	staticWidget.RegisterDirect("SetVisible", *this, &MainUI::SetVisible);
	staticWidget.RegisterDirect("GetText", *this, &MainUI::GetText);
	staticWidget.SetInteger("handle", buffSize);

	return staticWidget;
}

LuaPlus::LuaObject MainUI::GetText(LuaPlus::LuaObject thisEditBoxInstance)
{
	LuaPlus::LuaObject res;
	if (thisEditBoxInstance.IsTable()) {
		LuaPlus::LuaObject handleIndex = thisEditBoxInstance.GetByName("handle");
		if (handleIndex.IsInteger()) {
			int temp = handleIndex.GetInteger();
			CDXUTControl* pointer = m_HandleBuffer[temp];
			if (CDXUTEditBox* editBoxPointer = dynamic_cast<CDXUTEditBox*>(pointer)) {
				res.AssignString(LuaStateManager::Get()->GetLuaState(), ws2s(editBoxPointer->GetText()).c_str());
				return res;
			}
		}
	}
	return res;
}

LuaPlus::LuaObject MainUI::AddButton(const char* name, int x, int y, int width, int height)
{
	int buffSize = m_HandleBuffer.size();
	std::wstring staticNameW(s2ws(name));
	m_SampleUI.AddButton(buffSize, staticNameW.c_str(), x, y, width, height);
	CDXUTControl* widget = m_SampleUI.GetButton(buffSize);
	m_HandleBuffer.push_back(widget);

	LuaPlus::LuaObject staticWidget;
	staticWidget.AssignNewTable(LuaStateManager::Get()->GetLuaState());

	staticWidget.RegisterDirect("RegisterCallBack", *this, &MainUI::RegisterCallBack);
	staticWidget.RegisterDirect("SetVisible", *this, &MainUI::SetVisible);
	staticWidget.SetInteger("handle", buffSize);

	return staticWidget;
}

#pragma region UIView

UIView::UIView() : HumanView(shared_ptr<IRenderer>())
{
	m_MainMenuUI.reset(GCC_NEW MainUI);
	VPushElement(m_MainMenuUI);
}

UIView::~UIView() {}

void UIView::VRenderText()
{
	HumanView::VRenderText();
}


void UIView::VOnUpdate(unsigned long deltaMs)
{
	HumanView::VOnUpdate(deltaMs);
}

LRESULT CALLBACK UIView::VOnMsgProc(AppMsg msg)
{
	if (m_MainMenuUI->VIsVisible())
	{
		if (HumanView::VOnMsgProc(msg))
			return 1;
	}
	return 0;
}
#pragma endregion