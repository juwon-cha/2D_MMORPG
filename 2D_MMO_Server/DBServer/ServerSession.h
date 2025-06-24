#pragma once

#include "Session.h"

class ServerSession : public PacketSession
{
	virtual void OnConnected() override;
	virtual void OnSend(int32 len) override;
	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnDisconnected() override;
};
