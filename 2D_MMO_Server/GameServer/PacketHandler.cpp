#include "pch.h"
#include "PacketHandler.h"
#include "ClientSession.h"
#include "Player.h"
#include "GameRoom.h"
#include "PacketManager.h"

// [ 패킷 크기 ushort 2byte][ 패킷 종류 ushort  ] [uint64][uint64][ushort]

void PacketHandler::C_MOVEHandler(PacketSession* session, ByteRef buffer)
{

}
