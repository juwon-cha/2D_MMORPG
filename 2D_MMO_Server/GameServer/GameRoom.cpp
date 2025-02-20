#include "pch.h"
#include "GameRoom.h"
#include "Player.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "PacketManager.h"

int32 GameRoom::GetRoomId() const
{
    return _roomId;
}

void GameRoom::SetRoomId(int32 roomId)
{
    _roomId = roomId;
}

void GameRoom::EnterGame(shared_ptr<Player> newPlayer)
{
    if (newPlayer == nullptr)
    {
        return;
    }

    WRITE_LOCK;
    {
        _players.insert(pair<uint32, shared_ptr<Player>>(newPlayer->GetPlayerId(), newPlayer));
        newPlayer->SetGameRoom(shared_from_this());

#pragma region SC_ENTER_GAME
        // ���ο��� ���� ����
        {
            flatbuffers::FlatBufferBuilder builder;

            // ������ ���� ���� �����
            auto playerName = builder.CreateString(newPlayer->GetPlayerName());
            auto posInfo = CreatePositionInfo(builder, ObjectState_IDLE, MoveDir_NONE, newPlayer->GetPlayerPosX(), newPlayer->GetPlayerPosY());
            auto playerInfo = CreatePlayerInfo(builder, newPlayer->GetPlayerId(), playerName, posInfo);

            auto enter = CreateSC_ENTER_GAME(builder, playerInfo);
            auto enterPkt = PacketManager::Instance().CreatePacket(enter, builder, PacketType_SC_ENTER_GAME);

            newPlayer->GetClientSession()->Send(enterPkt);
        }
#pragma endregion

#pragma region SC_SPAWN
        // ���ο��� Ÿ�ε��� ���� ����
        {
            flatbuffers::FlatBufferBuilder builder;
            vector<flatbuffers::Offset<PlayerInfo>> infoArray;

            // infoArray�� Ÿ�� ���� ����
            for (const auto& pair : _players)
            {
                // �ڽ��� ���� �����ϰ� Ÿ�� ������ ����
                if (newPlayer != pair.second)
                {
                    auto playerName = builder.CreateString(pair.second->GetPlayerName());
                    auto posInfo = CreatePositionInfo(builder, pair.second->GetPlayerState(), pair.second->GetPlayerMoveDir(), pair.second->GetPlayerPosX(), pair.second->GetPlayerPosY());
                    auto playerInfo = CreatePlayerInfo(builder, pair.second->GetPlayerId(), playerName, posInfo);
                    infoArray.push_back(playerInfo);
                }
            }

            auto data = builder.CreateVector(infoArray);
            auto spawn = CreateSC_SPAWN(builder, data);
            auto spawnPkt = PacketManager::Instance().CreatePacket(spawn, builder, PacketType_SC_SPAWN);

            newPlayer->GetClientSession()->Send(spawnPkt);
        }

        // Ÿ�ο��� �� ���� ����
        {
            flatbuffers::FlatBufferBuilder builder;
            vector<flatbuffers::Offset<PlayerInfo>> InfoArray;

            auto playerName = builder.CreateString(newPlayer->GetPlayerName());
            auto posInfo = CreatePositionInfo(builder, newPlayer->GetPlayerState(), newPlayer->GetPlayerMoveDir(), newPlayer->GetPlayerPosX(), newPlayer->GetPlayerPosY());
            auto playerInfo = CreatePlayerInfo(builder, newPlayer->GetPlayerId(), playerName, posInfo);
            InfoArray.push_back(playerInfo);

            auto data = builder.CreateVector(InfoArray);
            auto spawn = CreateSC_SPAWN(builder, data);
            auto spawnPkt = PacketManager::Instance().CreatePacket(spawn, builder, PacketType_SC_SPAWN);

            for (const auto& pair : _players)
            {
                if (newPlayer != pair.second)
                {
                    pair.second->GetClientSession()->Send(spawnPkt);
                }
            }
        }
#pragma endregion
    }
}

void GameRoom::LeaveGame(int32 playerId)
{
    WRITE_LOCK;
    shared_ptr<Player> player;

    auto iter = _players.find(playerId);
    if (iter != _players.end())
    {
        player = iter->second;
        _players.erase(playerId);
        player->SetGameRoom(nullptr);
    }

#pragma region SC_LEAVE_GAME
    // �������� ����
    {
        flatbuffers::FlatBufferBuilder builder;

        auto leave = CreateSC_LEAVE_GAME(builder);
        auto leavePkt = PacketManager::Instance().CreatePacket(leave, builder, PacketType_SC_LEAVE_GAME);
        player->GetClientSession()->Send(leavePkt);
    }
#pragma endregion

#pragma region SC_DESPAWN
    // Ÿ������ �� ���� ����
    {
        flatbuffers::FlatBufferBuilder builder;
        vector<int32> idArray;
        idArray.push_back(player->GetPlayerId());

        flatbuffers::Offset<flatbuffers::Vector<int32>> data = builder.CreateVector(idArray);
        auto despawn = CreateSC_DESPAWN(builder, data);
        auto despawnPkt = PacketManager::Instance().CreatePacket(despawn, builder, PacketType_SC_DESPAWN);

        for (const auto& pair : _players)
        {
            // �ڽ��� �����ϰ� Ÿ�ο��Ը� ���� ����
            if (player != pair.second)
            {
                pair.second->GetClientSession()->Send(despawnPkt);
            }
        }
    }
#pragma endregion
}

void GameRoom::HandleMove(shared_ptr<Player> player, const C_MOVE* movePkt)
{
    if (player == nullptr)
    {
        return;
    }

    WRITE_LOCK; // �������� ��ǥ ����(�̵�)
    {
        // ���� ��ġ ����ȭ�� ���� �÷��̾� ���� ����
        // Ŭ���̾�Ʈ���� �̵��� �÷��̾��� ������ �����ϰ�,
        // GameRoom.cpp���� ���� �� ���� �÷��̾� ������ SC_SPAWN ��Ŷ�� ���� �� �÷��̾�� �ٸ� �÷��̾��� ��ǥ ����
        player->SetPlayerInfo(player->GetPlayerId(), player->GetPlayerName(), movePkt->posInfo()->posX(), movePkt->posInfo()->posY(), movePkt->posInfo()->state(), movePkt->posInfo()->moveDir());

        flatbuffers::FlatBufferBuilder builder;
        auto name = builder.CreateString(player->GetPlayerName());
        auto posInfo = CreatePositionInfo(builder, movePkt->posInfo()->state(), movePkt->posInfo()->moveDir(), movePkt->posInfo()->posX(), movePkt->posInfo()->posY());

        // �ٸ� �÷��̾�鿡�� �˷���
        auto move = CreateSC_MOVE(builder, player->GetPlayerId(), posInfo);
        auto respondMovePkt = PacketManager::Instance().CreatePacket(move, builder, PacketType_SC_MOVE);

        Broadcast(respondMovePkt);
    }
}

void GameRoom::Broadcast(SendBufferRef buffer)
{
    WRITE_LOCK;
    for (const auto& pair : _players)
    {
        pair.second->GetClientSession()->Send(buffer);
    }
}
