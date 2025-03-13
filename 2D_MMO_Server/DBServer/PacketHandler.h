#pragma once
#include "Session.h"

class PacketHandler
{
public:
	static void SD_SIGNUPHandler(PacketSession* session, ByteRef buffer);
	static void SD_SIGNINHandler(PacketSession* session, ByteRef buffer);
};
