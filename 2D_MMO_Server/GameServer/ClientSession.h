#pragma once
#include "Session.h"

class Player;

class ClientSession : public PacketSession
{
public:
	ClientSession();
	~ClientSession();

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override; // PacketSession의 OnRecvPacket을 오버라이드하여 모든 패킷 수신 시 시간 갱신
	virtual void OnSend(int32 len) override;

	void UpdateLastPacketReceiveTime();
	bool IsConnectionTimedOut(std::chrono::seconds timeoutDuration) const;

public:
	shared_ptr<Player> GetPlayer() const;
	void SetPlayer(shared_ptr<Player> player);

	std::chrono::steady_clock::time_point GetLastPacketReceiveTimeAtomic() const
	{ 
		return _lastPacketReceiveTime.load(std::memory_order_relaxed);
	}

public:
	uint64 SessionId;
	uint64 DbId = 0;

private:
	shared_ptr<Player> _player;
	atomic<chrono::steady_clock::time_point> _lastPacketReceiveTime;
};
