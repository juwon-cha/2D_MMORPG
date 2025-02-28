#include "pch.h"
#include "GameRoom.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "PacketManager.h"
#include "Map.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "Player.h"
#include "Monster.h"

GameRoom::GameRoom()
    : _roomId(0)
    , _map(nullptr)
{
    _map = make_shared<Map>();
}

void GameRoom::Init(int32 mapId)
{
    _map->LoadMap(mapId);

    // TEMP
    int32 monsterCount = 1;
    for (int32 i = 0; i < monsterCount; ++i)
    {
        shared_ptr<Monster> monster = ObjectManager::Instance().Add<Monster>();
        monster->SetObjectInfo(monster->GetObjectId(), "MONSTER_" + to_string(monster->GetObjectId()), 9, -9, ObjectState_IDLE, MoveDir_DOWN);
        EnterGame(monster);
    }
}

void GameRoom::EnterGame(shared_ptr<GameObject> gameObj)
{
    if (gameObj == nullptr)
    {
        return;
    }

    ObjectType type = ObjectManager::GetObjectTypeById(gameObj->GetObjectId());

    WRITE_LOCK;
    {
        if (type == ObjectType::PLAYER)
        {
            shared_ptr<Player> newPlayer = static_pointer_cast<Player>(gameObj);
            _players.insert(pair<uint32, shared_ptr<Player>>(newPlayer->GetObjectId(), newPlayer));
            newPlayer->SetGameRoom(shared_from_this());

            GetMap()->ApplyMove(newPlayer, Vector2Int(newPlayer->GetObjectPosX(), newPlayer->GetObjectPosY()));

            // 본인에게 정보 전송
            {
                flatbuffers::FlatBufferBuilder builder;

                // 전송할 본인 정보 만들기
                auto playerName = builder.CreateString(newPlayer->GetObjectName());
                auto posInfo = CreatePositionInfo(builder, ObjectState_IDLE, MoveDir_DOWN, newPlayer->GetObjectPosX(), newPlayer->GetObjectPosY());
                auto playerInfo = CreateObjectInfo(builder, newPlayer->GetObjectId(), playerName, posInfo);

                auto enter = CreateSC_ENTER_GAME(builder, playerInfo);
                auto enterPkt = PacketManager::Instance().CreatePacket(enter, builder, PacketType_SC_ENTER_GAME);

                newPlayer->GetClientSession()->Send(enterPkt);
            }

            // 본인에게 타인들의 정보 전송
            {
                flatbuffers::FlatBufferBuilder builder;
                vector<flatbuffers::Offset<ObjectInfo>> infoArray;

                // infoArray에 타인 정보 삽입
                for (const auto& pair : _players)
                {
                    // 자신의 정보 제외하고 타인 정보만 전송
                    if (newPlayer != pair.second)
                    {
                        auto playerName = builder.CreateString(pair.second->GetObjectName());
                        auto posInfo = CreatePositionInfo(builder, pair.second->GetObjectState(), pair.second->GetObjectMoveDir(), pair.second->GetObjectPosX(), pair.second->GetObjectPosY());
                        auto playerInfo = CreateObjectInfo(builder, pair.second->GetObjectId(), playerName, posInfo);
                        infoArray.push_back(playerInfo);
                    }
                }

                // infoArray에 몬스터 정보 삽입
                for (const auto& pair : _monsters)
                {
                    auto monsterName = builder.CreateString(pair.second->GetObjectName());
                    auto posInfo = CreatePositionInfo(builder, pair.second->GetObjectState(), pair.second->GetObjectMoveDir(), pair.second->GetObjectPosX(), pair.second->GetObjectPosY());
                    auto monsterInfo = CreateObjectInfo(builder, pair.second->GetObjectId(), monsterName, posInfo);
                    infoArray.push_back(monsterInfo);
                }

                auto data = builder.CreateVector(infoArray);
                auto spawn = CreateSC_SPAWN(builder, data);
                auto spawnPkt = PacketManager::Instance().CreatePacket(spawn, builder, PacketType_SC_SPAWN);

                newPlayer->GetClientSession()->Send(spawnPkt);
            }
        }
        else if (type == ObjectType::MONSTER)
        {
            // TODO: Monster Enter Game
            shared_ptr<Monster> monster = static_pointer_cast<Monster>(gameObj);
            _monsters.insert(pair<uint32, shared_ptr<Monster>>(monster->GetObjectId(), monster));
            monster->SetGameRoom(shared_from_this());

            GetMap()->ApplyMove(monster, Vector2Int(monster->GetObjectPosX(), monster->GetObjectPosY()));
        }
        else if (type == ObjectType::PROJECTILE)
        {

        }

        // 타인에게 내 정보 전송
        {
            flatbuffers::FlatBufferBuilder builder;
            vector<flatbuffers::Offset<ObjectInfo>> InfoArray;

            auto objectName = builder.CreateString(gameObj->GetObjectName());
            auto posInfo = CreatePositionInfo(builder, gameObj->GetObjectState(), gameObj->GetObjectMoveDir(), gameObj->GetObjectPosX(), gameObj->GetObjectPosY());
            auto objectInfo = CreateObjectInfo(builder, gameObj->GetObjectId(), objectName, posInfo);
            InfoArray.push_back(objectInfo);

            auto data = builder.CreateVector(InfoArray);
            auto spawn = CreateSC_SPAWN(builder, data);
            auto spawnPkt = PacketManager::Instance().CreatePacket(spawn, builder, PacketType_SC_SPAWN);

            for (const auto& pair : _players)
            {
                if (pair.second->GetObjectId() != gameObj->GetObjectId())
                {
                    pair.second->GetClientSession()->Send(spawnPkt);
                }
            }
        }
    }
}

void GameRoom::LeaveGame(int32 objectId)
{
    ObjectType type = ObjectManager::GetObjectTypeById(objectId);

    WRITE_LOCK;
    {
        if (type == ObjectType::PLAYER)
        {
            shared_ptr<Player> player = nullptr;

            auto iter = _players.find(objectId);
            if (iter != _players.end())
            {
                player = iter->second;
                _players.erase(objectId);

                player->SetGameRoom(nullptr);
                _map->ApplyLeave(player);
            }

            // 본인한테 전송
            {
                flatbuffers::FlatBufferBuilder builder;

                auto leave = CreateSC_LEAVE_GAME(builder);
                auto leavePkt = PacketManager::Instance().CreatePacket(leave, builder, PacketType_SC_LEAVE_GAME);
                player->GetClientSession()->Send(leavePkt);
            }
        }
        else if (type == ObjectType::MONSTER)
        {
            shared_ptr<Monster> monster = nullptr;
            auto iter = _monsters.find(objectId);
            if (iter != _monsters.end())
            {
                monster = iter->second;
                _monsters.erase(objectId);

                monster->SetGameRoom(nullptr);
                _map->ApplyLeave(monster);
            }
        }
        else if (type == ObjectType::PROJECTILE)
        {

        }

        // 타인한테 내 정보 전송
        {
            flatbuffers::FlatBufferBuilder builder;
            vector<int32> idArray;
            idArray.push_back(objectId);

            flatbuffers::Offset<flatbuffers::Vector<int32>> data = builder.CreateVector(idArray);
            auto despawn = CreateSC_DESPAWN(builder, data);
            auto despawnPkt = PacketManager::Instance().CreatePacket(despawn, builder, PacketType_SC_DESPAWN);

            for (const auto& pair : _players)
            {
                // 자신은 제외하고 타인에게만 정보 전송
                if (objectId != pair.second->GetObjectId())
                {
                    pair.second->GetClientSession()->Send(despawnPkt);
                }
            }
        }
    }
}

void GameRoom::HandleMove(shared_ptr<Player> player, const C_MOVE* movePkt)
{
    if (player == nullptr)
    {
        return;
    }

    WRITE_LOCK; // 서버에서 좌표 저장(이동)
    {
        // 클라이언트에서 받은 플레이어 정보 저장
        player->SetObjectInfo(player->GetObjectId(), player->GetObjectName(), player->GetObjectPosX(), player->GetObjectPosY(), movePkt->posInfo()->state(), movePkt->posInfo()->moveDir());

        // 이동 검증
        // 다른 좌표로 이동할 경우, 갈 수 있는지 체크
        if (movePkt->posInfo()->posX() != player->GetObjectPosX() || movePkt->posInfo()->posY() != player->GetObjectPosY())
        {
            if (_map->CanGo(Vector2Int(movePkt->posInfo()->posX(), movePkt->posInfo()->posY())) == false)
            {
                return;
            }
        }

        _map->ApplyMove(player, Vector2Int(movePkt->posInfo()->posX(), movePkt->posInfo()->posY()));

        flatbuffers::FlatBufferBuilder builder;
        auto name = builder.CreateString(player->GetObjectName());
        auto posInfo = CreatePositionInfo(builder, movePkt->posInfo()->state(), movePkt->posInfo()->moveDir(), movePkt->posInfo()->posX(), movePkt->posInfo()->posY());

        // 다른 플레이어들에게 알려줌
        auto move = CreateSC_MOVE(builder, player->GetObjectId(), posInfo);
        auto respondMovePkt = PacketManager::Instance().CreatePacket(move, builder, PacketType_SC_MOVE);

        Broadcast(respondMovePkt);
    }
}

void GameRoom::HandleSkill(shared_ptr<Player> player, const C_SKILL* skillPkt)
{
    if (player == nullptr)
    {
        return;
    }

    WRITE_LOCK;
    {
        if (player->GetObjectState() != ObjectState_IDLE)
        {
            return;
        }

        // TODO: Using skills verification

        // 플레이어 상태를 스킬 상태로 변경
        player->SetObjectInfo(player->GetObjectId(), player->GetObjectName(), player->GetObjectPosX(), player->GetObjectPosY(), player->GetObjectState(), player->GetObjectMoveDir());
        if (player->GetObjectState() != ObjectState_IDLE)
        {
            return;
        }
        player->SetObjectState(ObjectState_SKILL);

        flatbuffers::FlatBufferBuilder builder;
        int32 skillId = skillPkt->skillInfo()->skillId();
        auto skillInfo = CreateSkillInfo(builder, skillId);

        // 다른 플레이어들에게 알려줌
        auto skill = CreateSC_SKILL(builder, player->GetObjectId(), skillInfo);
        auto respondSkillPkt = PacketManager::Instance().CreatePacket(skill, builder, PacketType_SC_SKILL);

        Broadcast(respondSkillPkt);

        if (skillPkt->skillInfo()->skillId() == 1)
        {
            // TODO: Damaged
            Vector2Int skillPos = player->GetFrontCellPos(player->GetObjectMoveDir());
            shared_ptr<GameObject> target = _map->Find(skillPos);
            if (target != nullptr)
            {
                cout << target->GetObjectName() << " was hit by " << player->GetObjectName() << "!" << endl;
            }
        }
    }
}

shared_ptr<Player> GameRoom::FindPlayer(function<bool(shared_ptr<GameObject>)> condition)
{
    for (const auto& pair : _players)
    {
        if (condition(pair.second))
        {
            return pair.second;
        }
    }

    return nullptr;
}

void GameRoom::Broadcast(SendBufferRef buffer)
{
    //WRITE_LOCK;
    for (const auto& pair : _players)
    {
        pair.second->GetClientSession()->Send(buffer);
    }
}

void GameRoom::Update()
{
    //WRITE_LOCK;
    for (const auto& pair : _monsters)
    {
        pair.second->Update();
    }
}
