#include "pch.h"
#include "ServerSession.h"
#include "Manager.h"

void ServerSession::OnConnected()
{
	cout << "serversession onconnect\n";
	RegisterRecv();
}

void ServerSession::OnDisconnected()
{
}

void ServerSession::OnSend(int32 len)
{
	cout << "OnSend\n";
}

int32 ServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	Manager::Packet.OnRecvPacket(this, buffer);

	return len;
}
