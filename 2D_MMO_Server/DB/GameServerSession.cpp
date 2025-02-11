#include "GameServerSession.h"
#include "SessionManager.h"

void GameServerSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameServerSession>(shared_from_this()));
}

int32 GameServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	cout << "OnRecv Len = " << len << endl;

	PacketHeader header = *((PacketHeader*)buffer);
	cout << "Packet ID : " << header.id << " Size : " << header.size << endl;

	// Echo
	//Send(buffer, len);

	return len;
}

void GameServerSession::OnSend(int32 len)
{
}

void GameServerSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<GameServerSession>(shared_from_this()));
}
