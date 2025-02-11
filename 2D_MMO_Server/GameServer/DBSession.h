#pragma once
#include "Session.h"

class DBSession : public PacketSession
{
public:
	~DBSession()
	{
		cout << "~DBSession" << endl;
	}

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
};
