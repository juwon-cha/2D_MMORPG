#pragma once

#include "PacketManager.h"
#include "DbManager.h"
#include "ServerSession.h"
#include "Service.h"

class Manager
{
private:
	static shared_ptr<ServerService> service;
	Manager();

public:
	static ServerSession* session;
	static PacketManager& Packet;
	static DbManager& DB;

public:
	static void Init();
};