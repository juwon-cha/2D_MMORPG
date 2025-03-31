#include "pch.h"
#include "GameRoom.h"
#include "ClientSession.h"
#include "RoomManager.h"
#include "ClientSessionManager.h"
#include "PacketManager.h"
#include "Map.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "Player.h"
#include "Projectile.h"
#include "Monster.h"
#include "Item.h"
#include "ItemManager.h"
#include "DataManager.h"
#include "ContentsData.h"

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
        monster->SetObjectInfo(monster->GetObjectId(), "MONSTER_" + to_string(monster->GetObjectId()));
        monster->SetPosInfo(9, -9, ObjectState_IDLE, MoveDir_DOWN);
        monster->SetStatInfo(monster->GetObjectLevel(), monster->GetObjectSpeed(), monster->GetObjectHP(), monster->GetObjectMaxHP(), monster->GetObjectAttack(), monster->GetObjectTotalExp());
        EnterGame(monster);
    }
}

void GameRoom::EnterGame(shared_ptr<GameObject> gameObj)
{
    if (gameObj == nullptr)
    {
        return;
    }

    GameObjectType type = ObjectManager::GetObjectTypeById(gameObj->GetObjectId());

    WRITE_LOCK;
    {
        if (type == GameObjectType_PLAYER)
        {
            shared_ptr<Player> newPlayer = static_pointer_cast<Player>(gameObj);
            _players.insert(pair<int32, shared_ptr<Player>>(newPlayer->GetObjectId(), newPlayer));
            newPlayer->SetGameRoom(shared_from_this());

            GetMap()->ApplyMove(newPlayer, Vector2Int(newPlayer->GetObjectPosX(), newPlayer->GetObjectPosY()));

            // ���ο��� ���� ����
            {
                flatbuffers::FlatBufferBuilder builder;

                // ������ ���� ���� �����
                auto playerName = builder.CreateString(newPlayer->GetObjectName());
                auto posInfo = CreatePositionInfo(builder, ObjectState_IDLE, MoveDir_DOWN, newPlayer->GetObjectPosX(), newPlayer->GetObjectPosY());
                auto statInfo = CreateStatInfo(builder, newPlayer->GetObjectHP(), newPlayer->GetObjectMaxHP(), newPlayer->GetObjectSpeed());
                auto playerInfo = CreateObjectInfo(builder, newPlayer->GetObjectId(), playerName, posInfo, statInfo);

                auto enter = CreateSC_ENTER_GAME(builder, playerInfo);
                auto enterPkt = PacketManager::Instance().CreatePacket(enter, builder, PacketType_SC_ENTER_GAME);

                newPlayer->GetClientSession()->Send(enterPkt);
            }

            // ���ο��� Ÿ�ε��� ���� ����
            {
                flatbuffers::FlatBufferBuilder builder;
                vector<flatbuffers::Offset<ObjectInfo>> infoArray;

                // infoArray�� Ÿ�� ���� ����
                for (const auto& pair : _players)
                {
                    // �ڽ��� ���� �����ϰ� Ÿ�� ������ ����
                    if (newPlayer != pair.second)
                    {
                        auto playerName = builder.CreateString(pair.second->GetObjectName());
                        auto posInfo = CreatePositionInfo(builder, pair.second->GetObjectState(), pair.second->GetObjectMoveDir(), pair.second->GetObjectPosX(), pair.second->GetObjectPosY());
                        auto statInfo = CreateStatInfo(builder, pair.second->GetObjectHP(), pair.second->GetObjectMaxHP(), pair.second->GetObjectSpeed());
                        auto playerInfo = CreateObjectInfo(builder, pair.second->GetObjectId(), playerName, posInfo, statInfo);
                        infoArray.push_back(playerInfo);
                    }
                }

                // infoArray�� ���� ���� ����
                for (const auto& pair : _monsters)
                {
                    auto monsterName = builder.CreateString(pair.second->GetObjectName());
                    auto posInfo = CreatePositionInfo(builder, pair.second->GetObjectState(), pair.second->GetObjectMoveDir(), pair.second->GetObjectPosX(), pair.second->GetObjectPosY());
                    auto statInfo = CreateStatInfo(builder, pair.second->GetObjectHP(), pair.second->GetObjectMaxHP(), pair.second->GetObjectSpeed());
                    auto monsterInfo = CreateObjectInfo(builder, pair.second->GetObjectId(), monsterName, posInfo, statInfo);
                    infoArray.push_back(monsterInfo);
                }

                // infoArray�� ����ü ���� ����
                for (const auto& pair : _projectiles)
                {
                    auto projectileName = builder.CreateString(pair.second->GetObjectName());
                    auto posInfo = CreatePositionInfo(builder, pair.second->GetObjectState(), pair.second->GetObjectMoveDir(), pair.second->GetObjectPosX(), pair.second->GetObjectPosY());
                    auto statInfo = CreateStatInfo(builder, pair.second->GetObjectHP(), pair.second->GetObjectMaxHP(), pair.second->GetObjectSpeed());
                    auto projectileInfo = CreateObjectInfo(builder, pair.second->GetObjectId(), projectileName, posInfo, statInfo);
                    infoArray.push_back(projectileInfo);
                }

                auto data = builder.CreateVector(infoArray);
                auto spawn = CreateSC_SPAWN(builder, data);
                auto spawnPkt = PacketManager::Instance().CreatePacket(spawn, builder, PacketType_SC_SPAWN);

                newPlayer->GetClientSession()->Send(spawnPkt);
            }
        }
        else if (type == GameObjectType_MONSTER)
        {
            // Monster Enter Game
            shared_ptr<Monster> monster = static_pointer_cast<Monster>(gameObj);
            _monsters.insert(pair<int32, shared_ptr<Monster>>(monster->GetObjectId(), monster));
            monster->SetGameRoom(shared_from_this());

            GetMap()->ApplyMove(monster, Vector2Int(monster->GetObjectPosX(), monster->GetObjectPosY()));
        }
        else if (type == GameObjectType_PROJECTILE)
        {
            // Projectile Enter Game
            shared_ptr<Projectile> projectile = static_pointer_cast<Projectile>(gameObj);
            _projectiles.insert(pair<int32, shared_ptr<Projectile>>(projectile->GetObjectId(), projectile));
            projectile->SetGameRoom(shared_from_this());
        }

        // Ÿ�ο��� �� ���� ����
        {
            flatbuffers::FlatBufferBuilder builder;
            vector<flatbuffers::Offset<ObjectInfo>> InfoArray;

            auto objectName = builder.CreateString(gameObj->GetObjectName());
            auto posInfo = CreatePositionInfo(builder, gameObj->GetObjectState(), gameObj->GetObjectMoveDir(), gameObj->GetObjectPosX(), gameObj->GetObjectPosY());
            auto statInfo = CreateStatInfo(builder, gameObj->GetObjectHP(), gameObj->GetObjectMaxHP(), gameObj->GetObjectSpeed());
            auto objectInfo = CreateObjectInfo(builder, gameObj->GetObjectId(), objectName, posInfo, statInfo);
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
    GameObjectType type = ObjectManager::GetObjectTypeById(objectId);

    WRITE_LOCK;
    {
        if (type == GameObjectType_PLAYER)
        {
            shared_ptr<Player> player = nullptr;

            auto iter = _players.find(objectId);
            if (iter != _players.end())
            {
                player = iter->second;
                if (_players.erase(objectId) <= 0)
                {
                    return;
                }

                player->SetGameRoom(nullptr);
                _map->ApplyLeave(player);
            }

            // �������� ����
            {
                flatbuffers::FlatBufferBuilder builder;

                auto leave = CreateSC_LEAVE_GAME(builder);
                auto leavePkt = PacketManager::Instance().CreatePacket(leave, builder, PacketType_SC_LEAVE_GAME);
                player->GetClientSession()->Send(leavePkt);
            }
        }
        else if (type == GameObjectType_MONSTER)
        {
            shared_ptr<Monster> monster = nullptr;
            auto iter = _monsters.find(objectId);
            if (iter != _monsters.end())
            {
                monster = iter->second;
                if (_monsters.erase(objectId) <= 0)
                {
                    return;
                }

                monster->SetGameRoom(nullptr);
                _map->ApplyLeave(monster);
            }
        }
        else if (type == GameObjectType_PROJECTILE)
        {
            shared_ptr<Projectile> projectile = nullptr;
            auto iter = _projectiles.find(objectId);
            if (iter != _projectiles.end())
            {
                projectile = iter->second;
                if (_projectiles.erase(objectId) <= 0)
                {
                    return;
                }

                projectile->SetGameRoom(nullptr);
                _map->ApplyLeave(projectile);
            }
        }

        // Ÿ������ �� ���� ����
        {
            flatbuffers::FlatBufferBuilder builder;
            vector<int32> idArray;
            idArray.push_back(objectId);

            flatbuffers::Offset<flatbuffers::Vector<int32>> data = builder.CreateVector(idArray);
            auto despawn = CreateSC_DESPAWN(builder, data);
            auto despawnPkt = PacketManager::Instance().CreatePacket(despawn, builder, PacketType_SC_DESPAWN);

            for (const auto& pair : _players)
            {
                // �ڽ��� �����ϰ� Ÿ�ο��Ը� ���� ����
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

    WRITE_LOCK; // �������� ��ǥ ����(�̵�)
    {
        // Ŭ���̾�Ʈ���� ���� �÷��̾� ���� ����
        player->SetObjectInfo(player->GetObjectId(), player->GetObjectName());
        player->SetPosInfo(player->GetObjectPosX(), player->GetObjectPosY(), movePkt->posInfo()->state(), movePkt->posInfo()->moveDir());

        // �̵� ����
        // �ٸ� ��ǥ�� �̵��� ���, �� �� �ִ��� üũ
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

        // �ٸ� �÷��̾�鿡�� �˷���
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

        // �÷��̾� ���¸� ��ų ���·� ����
        player->SetObjectInfo(player->GetObjectId(), player->GetObjectName());
        player->SetPosInfo(player->GetObjectPosX(), player->GetObjectPosY(), player->GetObjectState(), player->GetObjectMoveDir());
        if (player->GetObjectState() != ObjectState_IDLE)
        {
            return;
        }
        player->SetObjectState(ObjectState_SKILL);

        flatbuffers::FlatBufferBuilder builder;
        int32 skillId = skillPkt->skillInfo()->skillId();
        auto skillInfo = CreateSkillInfo(builder, skillId);

        // �ٸ� �÷��̾�鿡�� �˷���
        auto skill = CreateSC_SKILL(builder, player->GetObjectId(), skillInfo);
        auto respondSkillPkt = PacketManager::Instance().CreatePacket(skill, builder, PacketType_SC_SKILL);

        Broadcast(respondSkillPkt);

        Skill skillData;
        auto iter = DataManager::Skills.find(skillPkt->skillInfo()->skillId());
        if (iter != DataManager::Skills.end())
        {
            skillData = iter->second;
        }

        switch (skillData.SkillType)
        {
        case SkillType_SKILL_AUTO:
        {
            Vector2Int skillPos = player->GetFrontCellPos(player->GetObjectMoveDir());
            shared_ptr<GameObject> target = _map->Find(skillPos);
            if (target != nullptr)
            {
                // NON PVP
                if (target->GetObjectType() == GameObjectType_PLAYER)
                {
                    return;
                }

                cout << target->GetObjectName() << " was hit!" << endl;
                target->OnDamaged(player, skillData.Damage + player->GetObjectAttack()/*��ų ���ݷ� + �÷��̾� ���ݷ�*/);
            }
        }
        break;

        case SkillType_SKILL_PROJECTILE:
        {
            shared_ptr<Projectile> projectile = ObjectManager::Instance().Add<Projectile>();
            if (projectile == nullptr)
            {
                return;
            }

            // �÷��̾� �տ� ȭ�� ����
            Vector2Int FrontCell = player->GetFrontCellPos();
            projectile->SetOwner(player);
            projectile->SetOwnerPos(player->GetObjectPosX(), player->GetObjectPosY());
            projectile->SetSkillData(skillData);
            projectile->SetObjectInfo(projectile->GetObjectId(), "Projectile" + to_string(projectile->GetObjectId()));
            projectile->SetPosInfo(FrontCell.X, FrontCell.Y, ObjectState_MOVING, player->GetObjectMoveDir());
            projectile->SetObjectSpeed(skillData.Projectile.Speed);
            EnterGame(projectile);
        }
        break;

        case SkillType_SKILL_NONE:
            return;
        }
    }
}

void GameRoom::HandleEquipItem(shared_ptr<Player> player, const C_EQUIP_ITEM* equipItemPkt)
{
    if (player == nullptr)
    {
        return;
    }

    shared_ptr<Item> item = player->GetInventory()->GetItem(equipItemPkt->itemId());
    if (item == nullptr)
    {
        return;
    }

    // ��� ����
    item->SetEquipped(equipItemPkt->equipped());

    // ������ ��� ���� ����
    map <int32, shared_ptr<Item>> playerItemList = player->GetInventory()->GetItemList();
    for (const auto& i : playerItemList)
    {
        if (i.second->GetObjectId() != item->GetObjectId())
        {
            i.second->SetEquipped(false);
        }
    }

    WRITE_LOCK;
    // Ŭ���̾�Ʈ�� ����
    flatbuffers::FlatBufferBuilder builder;

    auto equipItem = CreateSC_EQUIP_ITEM(builder, equipItemPkt->itemId(), equipItemPkt->equipped());
    auto equipOkPkt = PacketManager::Instance().CreatePacket(equipItem, builder, PacketType_SC_EQUIP_ITEM);

    player->GetClientSession()->Send(equipOkPkt);
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

void GameRoom::HandleChangeMap(shared_ptr<Player> player, const C_CHANGE_MAP* changeMapPkt)
{
    if (player == nullptr)
    {
        return;
    }

    if (player->GetGameRoom()->GetRoomId() != changeMapPkt->mapId())
    {
        return;
    }

    // ��ȯ�� �ʿ��� ������ ��ǥ
    int spawnX = 0;
    int spawnY = 0;

    if (changeMapPkt->mapId() == 1)
    {
        if ((player->GetObjectPosX() == -18 || player->GetObjectPosX() == -17 || player->GetObjectPosX() == -16) && player->GetObjectPosY() == 15)
        {
            player->SetMapId(5); // 5�� �� ������ ���� ����
            spawnX = -3;
            spawnY = -20;
        }
        else if (player->GetObjectPosX() == 18 && (player->GetObjectPosY() == 4 || player->GetObjectPosY() == 5 || player->GetObjectPosY() == 6))
        {
            player->SetMapId(4);
            spawnX = -31;
            spawnY = 12;
        }
        else if ((player->GetObjectPosX() == -3 || player->GetObjectPosX() == -2 || player->GetObjectPosX() == -1) && player->GetObjectPosY() == -17)
        {
            player->SetMapId(2);
            spawnX = -2;
            spawnY = 23;
        }
        else
        {
            return;
        }
    }
    else if (changeMapPkt->mapId() == 2)
    {
        if ((player->GetObjectPosX() == -3 || player->GetObjectPosX() == -2 || player->GetObjectPosX() == -1) && player->GetObjectPosY() == 24)
        {
            player->SetMapId(1);
            spawnX = -2;
            spawnY = -16;
        }
        else if (player->GetObjectPosX() == 29 && (player->GetObjectPosY() == 18 || player->GetObjectPosY() == 19 || player->GetObjectPosY() == 20))
        {
            player->SetMapId(3);
            spawnX = -31;
            spawnY = 19;
        }
        else
        {
            return;
        }
    }
    else if (changeMapPkt->mapId() == 3)
    {
        if (player->GetObjectPosX() == -32 && (player->GetObjectPosY() == 18 || player->GetObjectPosY() == 19 || player->GetObjectPosY() == 20))
        {
            player->SetMapId(2);
            spawnX = 28;
            spawnY = 19;
        }
        else if ((player->GetObjectPosX() == -9 || player->GetObjectPosX() == -10 || player->GetObjectPosX() == -11) && player->GetObjectPosY() == 24)
        {
            player->SetMapId(4);
            spawnX = -10;
            spawnY = -9;
        }
        else
        {
            return;
        }
    }
    else if (changeMapPkt->mapId() == 4)
    {
        if (player->GetObjectPosX() == -32 && (player->GetObjectPosY() == 11 || player->GetObjectPosY() == 12 || player->GetObjectPosY() == 13))
        {
            player->SetMapId(1);
            spawnX = 17;
            spawnY = 5;
        }
        else if ((player->GetObjectPosX() == -9 || player->GetObjectPosX() == -10 || player->GetObjectPosX() == -11) && player->GetObjectPosY() == -10)
        {
            player->SetMapId(3);
            spawnX = -10;
            spawnY = 23;
        }
        else
        {
            return;
        }
    }
    else if (changeMapPkt->mapId() == 5)
    {
        if ((player->GetObjectPosX() == -2 || player->GetObjectPosX() == -3 || player->GetObjectPosX() == -4) && player->GetObjectPosY() == -21)
        {
            player->SetMapId(1);
            spawnX = -17;
            spawnY = 14;
        }
        else
        {
            return;
        }
    }

    WRITE_LOCK;
    // ���� ��(���� ��)���� ����
    shared_ptr<Player> changeMapPlayer = player;
    LeaveGame(player->GetObjectId());

    // �ʿ� �������� �� �ٸ� �÷��̾�鿡�� �� ��ǥ�� ����ȭ�����ֱ� ����
    changeMapPlayer->SetObjectPosX(spawnX);
    changeMapPlayer->SetObjectPosY(spawnY);

    // ���� ������ ����
    RoomManager::Instance().Find(changeMapPlayer->GetMapId())->EnterGame(changeMapPlayer);

    flatbuffers::FlatBufferBuilder builder;

    auto map = CreateSC_CHANGE_MAP(builder, changeMapPlayer->GetMapId());
    auto mapPkt = PacketManager::Instance().CreatePacket(map, builder, PacketType_SC_CHANGE_MAP);

    player->GetClientSession()->Send(mapPkt);
}

void GameRoom::Broadcast(SendBufferRef buffer)
{
    //WRITE_LOCK;
    {
        for (const auto& pair : _players)
        {
            pair.second->GetClientSession()->Send(buffer);
        }
    }
}

void GameRoom::Update()
{
    //WRITE_LOCK;
    {
        for (const auto& pair : _monsters)
        {
            pair.second->Update();
        }

        // std::map ��ȯ������ ���� ���� �� begin()���� �����ϸ� ����
        // ������ iterator�� ����Ű�� �־ ��Ÿ�� ����
        // ���� �տ��� ���Ҹ� ����� �ڿ������� ��ȸ�ؼ� ����ü ������Ʈ
        for (auto iter = _projectiles.rbegin(); iter != _projectiles.rend(); ++iter)
        {
            iter->second->Update();

            if (_projectiles.size() == 0)
            {
                return;
            }
        }
    }
}
