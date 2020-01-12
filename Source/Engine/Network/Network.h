#pragma once
// Network.h - the core classes for creating a multiplayer game

#include <sys/types.h>
#include <Winsock2.h>
#include "..\EventManager\EventManager.h"


#define MAX_PACKET_SIZE (256)
#define RECV_BUFFER_SIZE (MAX_PACKET_SIZE * 512)
#define MAX_QUEUE_PER_PLAYER (10000)

#define MAGIC_NUMBER		(0x1f2e3d4c)
#define IPMANGLE(a,b,c,d) (((a)<<24)|((b)<<16)|((c)<<8)|((d)))
#define INVALID_SOCKET_ID (-1)

class NetSocket;

////////////////////////////////////////////////////
//
// IPacket Description
//
//    The interface class that defines a public API for 
//    packet objects - data that is either about to be
//    sent to or just been recieved from the network
//
////////////////////////////////////////////////////

class IPacket
{
public:
	virtual char const * const VGetType() const=0;
	virtual char const * const VGetData() const=0;
	virtual u_long VGetSize() const =0;
	virtual ~IPacket() { }
};

////////////////////////////////////////////////////
// class BinaryPacket							- 
//
//   A packet object that can be constructed all at once,
//   or with repeated calls to MemCpy
////////////////////////////////////////////////////

class BinaryPacket : public IPacket
{
protected:
	char *m_Data;

public:
	inline BinaryPacket(char const * const data, u_long size);
	inline BinaryPacket(u_long size);
	virtual ~BinaryPacket() { SAFE_DELETE(m_Data); }
	virtual char const * const VGetType() const { return g_Type; }
	virtual char const * const VGetData() const { return m_Data; }
	virtual u_long VGetSize() const { return ntohl(*(u_long *)m_Data); }
	inline void MemCpy(char const *const data, size_t size, int destOffset);

	static const char *g_Type;
};

///////////////////////////////////////////////////////////////////////////////
// BinaryPacket::BinaryPacket					- 
//
inline BinaryPacket::BinaryPacket(char const * const data, u_long size)
{
	m_Data = GCC_NEW char[size + sizeof(u_long)];
	GCC_ASSERT(m_Data);
	*(u_long *)m_Data = htonl(size+sizeof(u_long));
	memcpy(m_Data+sizeof(u_long), data, size);
}

inline BinaryPacket::BinaryPacket(u_long size)
{
	m_Data = GCC_NEW char[size + sizeof(u_long)];
	GCC_ASSERT(m_Data);
	*(u_long *)m_Data = htonl(size+sizeof(u_long));
}

///////////////////////////////////////////////////////////////////////////////
// BinaryPacket::MemCpy							- 
//
inline void BinaryPacket::MemCpy(char const *const data, size_t size, int destOffset)
{
	GCC_ASSERT(size+destOffset <= VGetSize()-sizeof(u_long));
	memcpy(m_Data + destOffset + sizeof(u_long), data, size);
}

////////////////////////////////////////////////////
// TextPacket Description						- not described in the book
//
//   A packet object that takes a text string.
////////////////////////////////////////////////////

class TextPacket : public BinaryPacket
{
public:
	TextPacket(char const * const text);
	virtual char const * const VGetType() const { return g_Type; }

	static const char *g_Type;
};


///////////////////////////////////////////////////////////////////////////////
// NetSocket Description						- 
//
//   A base class for a socket connection.
////////////////////////////////////////////////////

class NetSocket 
{
	friend class BaseSocketManager;
	typedef std::list< shared_ptr <IPacket> > PacketList;

public:
	NetSocket();											// clients use this to initialize a NetSocket prior to calling Connect.
	NetSocket(SOCKET new_sock, unsigned int hostIP);		// servers use this when new clients attach.
	virtual ~NetSocket();

	bool Connect(unsigned int ip, unsigned int port, bool forceCoalesce = 0);
	void SetBlocking(bool blocking);
	void Send(shared_ptr<IPacket> pkt, bool clearTimeOut=1);

	virtual int  VHasOutput() { return !m_OutList.empty(); }
	virtual void VHandleOutput();
	virtual void VHandleInput();
	virtual void VTimeOut() { m_timeOut=0; }

	void HandleException() { m_deleteFlag |= 1; }

	void SetTimeOut(unsigned int ms=45*1000) { m_timeOut = timeGetTime() + ms; }

	int GetIpAddress() { return m_ipaddr; }

protected:
    SOCKET m_sock;
	int m_id;				// a unique ID given by the socket manager

	// note: if deleteFlag has bit 2 set, exceptions only close the
	//   socket and set to INVALID_SOCKET, and do not delete the NetSocket
    int m_deleteFlag;
 
	PacketList m_OutList;
	PacketList m_InList;

	char m_recvBuf[RECV_BUFFER_SIZE];
	unsigned int m_recvOfs, m_recvBegin;
	bool m_bBinaryProtocol;

    int m_sendOfs;
	unsigned int m_timeOut;
	unsigned int m_ipaddr;

	int m_internal;
	int m_timeCreated;
};


//
// class BaseSocketManager						- 
//
class BaseSocketManager
{
protected:
	WSADATA m_WsaData;

	typedef std::list<NetSocket *> SocketList;
	typedef std::map<int, NetSocket *> SocketIdMap;

	SocketList m_SockList;
	SocketIdMap m_SockMap;

	int m_NextSocketId;
	unsigned int m_Inbound;
	unsigned int m_Outbound;
	unsigned int m_MaxOpenSockets;
	unsigned int m_SubnetMask;
	unsigned int m_Subnet;

	NetSocket *FindSocket(int sockId);

public:
	BaseSocketManager();
	virtual ~BaseSocketManager() { Shutdown(); }

	void DoSelect(int pauseMicroSecs, bool handleInput = true);

	bool Init();
	void Shutdown();
	void PrintError();

	int AddSocket(NetSocket *socket); 
	void RemoveSocket(NetSocket *socket);

	unsigned int GetHostByName(const std::string &hostName);
	const char *GetHostByAddr(unsigned int ip);

	int GetIpAddress(int sockId);

	void SetSubnet(unsigned int subnet, unsigned int subnetMask)
	{
		m_Subnet = subnet;
		m_SubnetMask = subnetMask;
	}
	bool IsInternal(unsigned int ipaddr);

	bool Send(int sockId, shared_ptr<IPacket> packet);

	void AddToOutbound(int rc) { m_Outbound += rc; }
	void AddToInbound(int rc) { m_Inbound += rc; }

};

extern BaseSocketManager *g_pSocketManager;


//
// class ClientSocketManager					- 
//
class ClientSocketManager : public BaseSocketManager
{
	std::string m_HostName;
	unsigned int m_Port;

public:
	ClientSocketManager(const std::string &hostName, unsigned int port)
	{
		m_HostName = hostName;
		m_Port = port;
	}

	bool Connect();
};





//
// class NetListenSocket						- 
//
class NetListenSocket: public NetSocket
{
public:
	NetListenSocket() { };
	NetListenSocket(int portnum);

	void Init(int portnum);
	void InitScan(int portnum_min, int portnum_max);
	SOCKET AcceptConnection(unsigned int *pAddr);

	unsigned short port;
};


//
// class GameServerListenSocket					- 
//
class GameServerListenSocket: public NetListenSocket 
{
public:
	GameServerListenSocket(int portnum) { Init(portnum); }

	virtual void VHandleInput();
};

//
// class RemoteEventSocket						- 
//
class RemoteEventSocket: public NetSocket 
{
public:
	enum
	{
		NetMsg_Event,
		NetMsg_PlayerLoginOk,
	};

	// server accepting a client
	RemoteEventSocket(SOCKET new_sock, unsigned int hostIP)		
	: NetSocket(new_sock, hostIP)
	{
	}

	// client attach to server
	RemoteEventSocket() { };										

	virtual void VHandleInput();

protected:
	void CreateEvent(std::istrstream &in);
};



//
// class NetworkEventForwarder					- 
//
// The only thing a network game view does is 
// act as an intermediary between the server and the 'real' remote view
// it listens to the same messages as a a game view
// and sends them along via TCP/IP

class NetworkEventForwarder
{
public:
	NetworkEventForwarder(int sockId) { m_SockId = sockId;  }

    // Delegate that forwards events through the network.  The game layer must register objects of this class for 
    // the events it wants.  See TeapotWarsApp::VCreateGameAndView() and TeapotWarsLogic::RemoteClientDelegate()
    // for examples of this happening.
    void ForwardEvent(IEventDataPtr pEventData);

protected:
	int m_SockId;
};


//
// class NetworkGameView						- 
//
class NetworkGameView : public IGameView
{

public:
	// IGameView Implementation - everything is stubbed out.
	virtual HRESULT VOnRestore() { return S_OK; }
	virtual void VOnRender(double fTime, float fElapsedTime) { }
	virtual HRESULT VOnLostDevice() { return S_OK; }
	virtual GameViewType VGetType() { return GameView_Remote; }
	virtual GameViewId VGetId() const { return m_ViewId; }
	virtual void VOnAttach(GameViewId vid, ActorId aid);
	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg ) { return 0; }
	virtual void VOnUpdate(unsigned long deltaMs);

	void NewActorDelegate(IEventDataPtr pEventData);

	void SetPlayerActorId(ActorId actorId) { m_ActorId = actorId; }
	void AttachRemotePlayer(int sockID);

	int HasRemotePlayerAttached() { return m_SockId != INVALID_SOCKET_ID; }

	NetworkGameView();

protected:
	GameViewId m_ViewId;
	ActorId m_ActorId;
	int m_SockId;
};










