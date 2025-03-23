#include "pch.h"
#include "PacketHandler.h"
#include "ClientSession.h"
#include "Player.h"
#include "GameRoom.h"
#include "PacketManager.h"

// [ ��Ŷ ũ�� ushort 2byte][ ��Ŷ ���� ushort  ] [uint64][uint64][ushort]

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
	try {
		auto pkt = GetRoot<C_CHAT>(buffer->operator unsigned char* ());

		ClientSession* clientSession = static_cast<ClientSession*>(session);
		auto player = clientSession->GetPlayer();
		if (player == nullptr)
		{
			session->Disconnect(L"INVALID CLIENT [C_CHAT]");
			return;
		}
		auto room = player->GetGameRoom();
		if (room == nullptr)
		{
			session->Disconnect(L"INVALID CLIENT [C_CHAT]");
			return;
		}
		FlatBufferBuilder builder;
		auto name = player->GetObjectName();
		auto text = pkt->text()->c_str();
		auto playerInfo = CreateObjectInfoDirect(builder, player->GetObjectId(), name.c_str());
		auto data = CreateSC_CHATDirect(builder, playerInfo, text);
		auto packet = PacketManager::Instance().CreatePacket(data, builder, PacketType_SC_CHAT);
		room->Broadcast(packet);
	}
	catch (...) {
		// ���� �α� �ʿ�
	}
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
