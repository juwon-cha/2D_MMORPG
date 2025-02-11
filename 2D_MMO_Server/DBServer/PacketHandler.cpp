#include "pch.h"
#include "PacketHandler.h"
#include "format.h"
#include "CoreRef.h"

void PacketHandler::SD_TestHandler(PacketSession* session, ByteRef buffer)
{
	auto pkt = GetRoot<SD_Test>(buffer->operator unsigned char* ());
	cout << ":! " << pkt->test_sd() << endl;
}
