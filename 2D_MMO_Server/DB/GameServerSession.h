#pragma once
#include "pch.h"
#include "Session.h"

class GameServerSession : public PacketSession
{
public:
	~GameServerSession()
	{
		cout << "~GameServerSession" << endl;
	}

	virtual void OnConnected() override;
	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
	virtual void OnDisconnected() override;
};

