#include "pch.h"
#include "DBSession.h"
#include "PacketManager.h"

void DBSession::OnConnected()
{
	cout << "DB Connected!" << endl;
	FlatBufferBuilder builder;

	auto data = CreateSD_Test(builder, 3);
	PacketManager& a = PacketManager::Instance();
	auto pkt = a.CreatePacket(data, builder, PacketType_SD_Test);
	Send(pkt);
}

void DBSession::OnDisconnected()
{
	cout << "DB Disonnected!" << endl;
}

int32 DBSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketManager::Instance().OnRecvPacket(this, buffer);

	return len;
}

void DBSession::OnSend(int32 len)
{
	//cout << len << endl;
}
