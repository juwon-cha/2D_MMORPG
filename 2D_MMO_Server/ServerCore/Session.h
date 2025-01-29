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
	// �ܺο��� ���
	void Send(shared_ptr<SendBuffer> sendBuffer);
	bool Connect();
	void Disconnect(const WCHAR* cause);

	shared_ptr<Service>	GetService() { return _service.lock(); }
	void SetService(shared_ptr<Service> service) { _service = service; }

public:
	// ���� ����
	void SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress GetAddress() const { return _netAddress; }
	SOCKET GetSocket() const { return _socket; }
	bool IsConnected() { return _connected; }
	shared_ptr<Session>	GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:
	// �������̽� ����
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	// ����
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
	// ������ �ڵ忡�� �������̵�
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

	// ����
	RecvBuffer _recvBuffer;

	// �۽�
	queue<shared_ptr<SendBuffer>> _sendQueue;
	atomic<bool> _sendRegistered = false;

private:
	// IocpEvent ����
	ConnectEvent _connectEvent;
	DisconnectEvent _disconnectEvent;
	RecvEvent _recvEvent;
	SendEvent _sendEvent;
};

// PacketSession
struct PacketHeader
{
	uint16 size; // ��� ���� ��Ŷ ��ü ũ��
	uint16 id; // ��������ID (ex. 1=�α���, 2=�̵���û)
};

/*
��Ŷ ���(�ּ� 4����Ʈ) �Ľ� -> ��Ŷ�� ũ�� ����
-> ���ݱ��� ���� �����Ͱ� ������ ��Ŷ�� ũ�⺸�� ū�� Ȯ��
-> ũ�ٸ� ��Ŷ ũ�⸸ŭ ó��
-> �۴ٸ� ��Ŷ ó�� �Ұ��� -> ������ ���������� ��ٸ�
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