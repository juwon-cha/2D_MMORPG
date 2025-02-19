#include "pch.h"
#include "PacketHandler.h"
#include "ClientSession.h"
#include "Player.h"
#include "GameRoom.h"
#include "PacketManager.h"

// [ 패킷 크기 ushort 2byte][ 패킷 종류 ushort  ] [uint64][uint64][ushort]

void PacketHandler::C_MOVEHandler(PacketSession* session, ByteRef buffer)
{
	ClientSession* clientSession = static_cast<ClientSession*>(session);
	auto movePkt = GetRoot<C_MOVE>(buffer->operator BYTE * ());
	cout << "C_MOVE (" << movePkt->posInfo()->posX() << ", " << movePkt->posInfo()->posY() << ")" << endl;

	shared_ptr<Player> player = clientSession->GetPlayer();
	if (player == nullptr)
	{
		return;
	}

	shared_ptr<GameRoom> room = player->GetGameRoom();
	if (room == nullptr)
	{
		return;
	}

	// 스폰 위치 동기화를 위한 플레이어 정보 저장
	// 클라이언트에서 이동한 플레이어의 정보를 저장하고,
	// GameRoom.cpp에서 게임 룸 안의 플레이어 정보를 SC_SPAWN 패킷을 통해 내 플레이어에게 다른 플레이어의 좌표 전송
	player->SetPlayerInfo(player->GetPlayerId(), player->GetPlayerName(), movePkt->posInfo()->posX(), movePkt->posInfo()->posY(), movePkt->posInfo()->state(), movePkt->posInfo()->moveDir());

	room->HandleMove(player, movePkt);
}
