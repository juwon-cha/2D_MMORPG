#include "pch.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"

void ClientSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<ClientSession>(shared_from_this()));
	cout << "OnConnected!\n";
}

void ClientSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));
}

int32 ClientSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	cout << "OnRecv Len = " << len << endl;

	PacketHeader header = *((PacketHeader*)buffer);
	cout << "Packet ID : " << header.id << " Size : " << header.size << endl;

	return len;
}

void ClientSession::OnSend(int32 len)
{
	//cout << "OnSend Len = " << len << endl;
}