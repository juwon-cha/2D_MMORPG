#pragma once
#include "Session.h"

class PacketHandler
{
public:
	static void C_MOVEHandler(PacketSession* session, ByteRef buffer);
	static void C_CHATHandler(PacketSession* session, ByteRef buffer);
	static void C_SKILLHandler(PacketSession* session, ByteRef buffer);
	static void C_SIGNUPHandler(PacketSession* session, ByteRef buffer);
	static void D_SIGNUPHandler(PacketSession* session, ByteRef buffer);
	static void C_SIGNINHandler(PacketSession* session, ByteRef buffer);
	static void D_SIGNINHandler(PacketSession* session, ByteRef buffer);
	static void C_ENTER_GAMEHandler(PacketSession* session, ByteRef buffer);
};