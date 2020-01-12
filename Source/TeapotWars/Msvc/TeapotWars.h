#pragma once
// TeapotWars.h : source file for the sample game

#pragma warning( disable : 4996 ) // 'function' declared deprecated - gets rid of all those 2005 warnings....

class PathingGraph;
class IEventManager;


//---------------------------------------------------------------------------------------------------------------------
// TeapotWarsApp class                          - 
// Note: Don't put anything in this class that needs to be destructed
//---------------------------------------------------------------------------------------------------------------------
class TeapotWarsApp : public Engine
{
protected:
	virtual BaseGameLogic *VCreateGameAndView();

public:
	virtual TCHAR *VGetGameTitle() { return _T("Teapot Wars"); }
	virtual TCHAR *VGetGameAppDirectory() { return _T("Game Coding Complete 4\\Teapot Wars\\4.0"); }
	virtual HICON VGetIcon();

protected:
    virtual void VRegisterGameEvents(void);
    virtual void VCreateNetworkEventForwarder(void);
    virtual void VDestroyNetworkEventForwarder(void);
};


//---------------------------------------------------------------------------------------------------------------------
// TeapotWarsLogic class                        - 
//---------------------------------------------------------------------------------------------------------------------
class TeapotWarsLogic : public BaseGameLogic
{
protected:
    std::list<NetworkEventForwarder*> m_networkEventForwarders;

public:
	TeapotWarsLogic();
	virtual ~TeapotWarsLogic();

	// TeapotWars Methods

	// Update
	virtual void VSetProxy();
    virtual void VMoveActor(const ActorId id, Mat4x4 const &mat);

	// Overloads
	virtual void VChangeState(BaseGameState newState);
	virtual void VAddView(shared_ptr<IGameView> pView, ActorId actorId=INVALID_ACTOR_ID);
	virtual shared_ptr<IGamePhysics> VGetGamePhysics(void) { return m_pPhysics; }

    // event delegates
    void RequestStartGameDelegate(IEventDataPtr pEventData);
    void RemoteClientDelegate(IEventDataPtr pEventData);
    void NetworkPlayerActorAssignmentDelegate(IEventDataPtr pEventData);
	void EnvironmentLoadedDelegate(IEventDataPtr pEventData);
    void ThrustDelegate(IEventDataPtr pEventData);
    void SteerDelegate(IEventDataPtr pEventData);
    void StartThrustDelegate(IEventDataPtr pEventData);
    void EndThrustDelegate(IEventDataPtr pEventData);
    void StartSteerDelegate(IEventDataPtr pEventData);
    void EndSteerDelegate(IEventDataPtr pEventData);

    void TestScriptDelegate(IEventDataPtr pEventData);

protected:
    virtual bool VLoadGameDelegate(TiXmlElement* pLevelData);

private:
    void RegisterAllDelegates(void);
    void RemoveAllDelegates(void);
    void CreateNetworkEventForwarder(const int socketId);
    void DestroyAllNetworkEventForwarders(void);
};
