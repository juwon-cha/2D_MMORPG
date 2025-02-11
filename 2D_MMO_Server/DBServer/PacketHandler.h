#pragma once
#include "Session.h"

class PacketHandler
{
public:
	static void SD_TestHandler(PacketSession* session, ByteRef buffer);
};
