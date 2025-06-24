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

	room->HandleMove(player, movePkt);
}

void PacketHandler::C_CHATHandler(PacketSession* session, ByteRef buffer)
{
}

void PacketHandler::C_SKILLHandler(PacketSession* session, ByteRef buffer)
{
	ClientSession* clientSession = static_cast<ClientSession*>(session);
	auto skillPkt = GetRoot<C_SKILL>(buffer->operator BYTE * ());

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

	room->HandleSkill(player, skillPkt);
}

void PacketHandler::C_EQUIP_ITEMHandler(PacketSession* session, ByteRef buffer)
{
	ClientSession* clientSession = static_cast<ClientSession*>(session);
	auto equipItemPkt = GetRoot<C_EQUIP_ITEM>(buffer->operator BYTE * ());

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

	room->HandleEquipItem(player, equipItemPkt);
}

void PacketHandler::C_CHANGE_MAPHandler(PacketSession* session, ByteRef buffer)
{
	ClientSession* clientSession = static_cast<ClientSession*>(session);
	auto changeMapPkt = GetRoot<C_CHANGE_MAP>(buffer->operator BYTE * ());

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

	room->HandleChangeMap(player, changeMapPkt);
}
