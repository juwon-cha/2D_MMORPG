#pragma once
#include "Session.h"

class PacketHandler
{
public:
	static void C_MOVEHandler(PacketSession* session, ByteRef buffer);
	static void C_ChatHandler(PacketSession* session, ByteRef buffer);
};