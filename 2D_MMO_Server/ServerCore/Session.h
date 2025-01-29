#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"

class Service;

class COREDLL Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000 // 64KB
	};

public:
	Session();
	virtual ~Session();

public:
	// 외부에서 사용
	void Send(shared_ptr<SendBuffer> sendBuffer);
	bool Connect();
	void Disconnect(const WCHAR* cause);

	shared_ptr<Service>	GetService() { return _service.lock(); }
	void SetService(shared_ptr<Service> service) { _service = service; }

public:
	// 정보 관련
	void SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress GetAddress() const { return _netAddress; }
	SOCKET GetSocket() const { return _socket; }
	bool IsConnected() { return _connected; }
	shared_ptr<Session>	GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:
	// 인터페이스 구현
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	// 전송
	bool RegisterConnect();
	bool RegisterDisconnect();
	void RegisterRecv();
	void RegisterSend();
		 
	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(int32 numOfBytes);
	void ProcessSend(int32 packetLen);
		 
	void HandleError(int32 errorCode);

protected:
	// 컨텐츠 코드에서 오버라이딩
	virtual void OnConnected() { }
	virtual int32 OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void OnSend(int32 len) { }
	virtual void OnDisconnected() { }

private:
	weak_ptr<Service> _service;
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	atomic<bool> _connected = false;

private:
	USE_LOCK;

	// 수신
	RecvBuffer _recvBuffer;

	// 송신
	queue<shared_ptr<SendBuffer>> _sendQueue;
	atomic<bool> _sendRegistered = false;

private:
	// IocpEvent 재사용
	ConnectEvent _connectEvent;
	DisconnectEvent _disconnectEvent;
	RecvEvent _recvEvent;
	SendEvent _sendEvent;
};

// PacketSession
struct PacketHeader
{
	uint16 size; // 헤더 포함 패킷 전체 크기
	uint16 id; // 프로토콜ID (ex. 1=로그인, 2=이동요청)
};

/*
패킷 헤더(최소 4바이트) 파싱 -> 패킷의 크기 추출
-> 지금까지 받은 데이터가 추출한 패킷의 크기보다 큰지 확인
-> 크다면 패킷 크기만큼 처리
-> 작다면 패킷 처리 불가능 -> 데이터 받을때까지 기다림
*/
// [size(2)][id(2)][data...][size(2)][id(2)][data...]...
class COREDLL PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

	shared_ptr<PacketSession> GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual int32 OnRecv(BYTE* buffer, int32 len) sealed;
	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) abstract;
};