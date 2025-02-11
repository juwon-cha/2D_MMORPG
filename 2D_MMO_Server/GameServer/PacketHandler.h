#pragma once

class PacketHandler
{
public:
	static void C_TestHandler(PacketSession* session, ByteRef buffer);
	static void D_TestHandler(PacketSession* session, ByteRef buffer);
	static void SD_TestHandler(PacketSession* session, ByteRef buffer);
};