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

void PacketHandler::C_ChatHandler(PacketSession* session, ByteRef buffer)
{
	try {
		auto pkt = GetRoot<C_Chat>(buffer->operator unsigned char* ());

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
		auto name = player->GetPlayerName();
		auto text = pkt->text()->c_str();
		auto playerInfo = CreatePlayerInfoDirect(builder, player->GetPlayerId(), name.c_str());
		auto data = CreateSC_ChatDirect(builder, playerInfo, text);
		auto packet = PacketManager::Instance().CreatePacket(data, builder, PacketType_SC_Chat);
		room->Broadcast(packet);
	}
	catch (...) {
		// 에러 로깅 필요
	}
}
