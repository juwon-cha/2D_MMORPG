#pragma once
#include "Session.h"

class Player;

class ClientSession : public PacketSession
{
public:
	uint64 SessionId;
	uint64 DbId = 0;
public:
	~ClientSession()
	{
		cout << "~ClientSession" << endl;
	}

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;

public:
	shared_ptr<Player> GetPlayer() const;
	void SetPlayer(shared_ptr<Player> player);

private:
	shared_ptr<Player> _player;
};
