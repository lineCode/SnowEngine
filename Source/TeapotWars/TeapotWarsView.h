#pragma once
// TeapotWarsView.h : source file for the sample game

#include <DXUTGui.h>

#include "../Engine/Scene/SceneNodes.h"
#include "../Engine/EventManager/EventManager.h"
#include "../Engine/UserInterface/HumanView.h"


class MainMenuUI : public BaseUI
{
protected:
	// dialog for sample specific controls
	CDXUTDialog m_SampleUI;					
	void Set();
	bool m_bCreatingGame;
	int m_NumAIs;
	int m_NumPlayers;
	std::vector<std::wstring> m_Levels;
	int m_LevelIndex;
	std::string m_HostName;
	std::string m_HostListenPort;
	std::string m_ClientAttachPort;

public:
	MainMenuUI();
	virtual ~MainMenuUI();

	// IScreenElement Implementation
	virtual HRESULT VOnRestore();
	virtual HRESULT VOnRender(double fTime, float fElapsedTime);
	virtual int VGetZOrder() const { return 1; }
	virtual void VSetZOrder(int const zOrder) { }

	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg );
	static void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext );
	void CALLBACK _OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext );

};


class StandardHUD : public BaseUI
{
protected:
	CDXUTDialog m_HUD;                  // dialog for standard controls

public:
	StandardHUD();
	virtual ~StandardHUD();

	// IScreenElement Implementation
	virtual HRESULT VOnRestore();
	virtual HRESULT VOnRender(double fTime, float fElapsedTime);
	virtual int VGetZOrder() const { return 1; }
	virtual void VSetZOrder(int const zOrder) { }

	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg );
	static void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void *pUserContext );
};

class IEventManager;
class SoundProcess;
class TeapotController;

class MainMenuView : public HumanView
{
protected:
	shared_ptr<MainMenuUI> m_MainMenuUI; 
public:

	MainMenuView(); 
	~MainMenuView(); 
	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg );
	virtual void VRenderText();	
	virtual void VOnUpdate(unsigned long deltaMs);
};

class TeapotWarsHumanView : public HumanView
{
protected:
	bool  m_bShowUI;					// If true, it renders the UI control text
    std::wstring m_gameplayText;

	shared_ptr<TeapotController> m_pTeapotController;
	shared_ptr<MovementController> m_pFreeCameraController;
	shared_ptr<SceneNode> m_pTeapot;
	shared_ptr<StandardHUD> m_StandardHUD; 

public:
	TeapotWarsHumanView(shared_ptr<IRenderer> renderer); 
	virtual ~TeapotWarsHumanView();

	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg );	
	virtual void VRenderText();	
	virtual void VOnUpdate(unsigned long deltaMs);
	virtual void VOnAttach(GameViewId vid, ActorId aid);

	virtual void VSetControlledActor(ActorId actorId);
	virtual bool VLoadGameDelegate(TiXmlElement* pLevelData) override;

    // event delegates
    void GameplayUiUpdateDelegate(IEventDataPtr pEventData);
    void SetControlledActorDelegate(IEventDataPtr pEventData);

private:
    void RegisterAllDelegates(void);
    void RemoveAllDelegates(void);
};


class AITeapotView : public IGameView 
{
	friend class AITeapotViewListener;

private:
	shared_ptr<PathingGraph> m_pPathingGraph;

protected:
	GameViewId	m_ViewId;
	ActorId m_PlayerActorId;

public:
	AITeapotView(shared_ptr<PathingGraph> pPathingGraph);
	virtual ~AITeapotView();

	virtual HRESULT VOnRestore() { return S_OK; }
	virtual void VOnRender(double fTime, float fElapsedTime) {}
	virtual HRESULT VOnLostDevice() { return S_OK; }
	virtual GameViewType VGetType() { return GameView_AI; }
	virtual GameViewId VGetId() const { return m_ViewId; }
	virtual void VOnAttach(GameViewId vid, ActorId actorId) { m_ViewId = vid; m_PlayerActorId = actorId; }
	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg ) {	return 0; }
	virtual void VOnUpdate(unsigned long deltaMs) {}
	
	shared_ptr<PathingGraph> GetPathingGraph(void) const { return m_pPathingGraph; }
};
