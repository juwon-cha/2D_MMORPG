#include "pch.h"
#include "DBSession.h"

void DBSession::OnConnected()
{
	cout << "DB Connected!" << endl;
}

void DBSession::OnDisconnected()
{
	cout << "DB Disonnected!" << endl;
}

int32 DBSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	return len;
}

void DBSession::OnSend(int32 len)
{
	//cout << len << endl;
}
