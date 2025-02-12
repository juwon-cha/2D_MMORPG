#pragma once

#include "PacketManager.h"
#include "DbManager.h"
#include "ServerSession.h"

class Manager
{
private:
	Manager();

public:
	static ServerSession* session;
	static PacketManager& Packet;
	static DbManager& DB;

public:
	static void Init();
};