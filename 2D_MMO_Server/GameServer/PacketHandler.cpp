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

	Player* player = clientSession->GetPlayer().get();
	if (player == nullptr)
	{
		return;
	}
	if (player->GetGameRoom() == nullptr)
	{
		return;
	}

	// �������� ��ǥ ����(�̵�)
	{
		flatbuffers::FlatBufferBuilder builder;
		auto name = builder.CreateString(player->GetPlayerName());
		auto posInfo = CreatePositionInfo(builder, movePkt->posInfo()->state(), movePkt->posInfo()->moveDir(), movePkt->posInfo()->posX(), movePkt->posInfo()->posY());

		// �ٸ� �÷��̾�鿡�� �˷���
		auto move = CreateSC_MOVE(builder, player->GetPlayerId(), posInfo);
		auto respondMovePkt = PacketManager::Instance().CreatePacket(move, builder, PacketType_SC_MOVE);

		player->GetGameRoom()->Broadcast(respondMovePkt);
	}
}
