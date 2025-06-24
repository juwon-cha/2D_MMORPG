#include "pch.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "Player.h"

ObjectManager* ObjectManager::_instance = nullptr;

ObjectManager& ObjectManager::Instance()
{
	if (_instance == nullptr)
	{
		_instance = new ObjectManager();
	}

	return *_instance;
}

bool ObjectManager::Remove(int32 objectId)
{
	GameObjectType objType = GetObjectTypeById(objectId);

	WRITE_LOCK;
	{
		if (objType == GameObjectType_PLAYER)
		{
			return _players.erase(objectId);
		}
	}
	
	return false;
}

shared_ptr<GameObject> ObjectManager::Find(int32 objectId)
{
	GameObjectType objType = GetObjectTypeById(objectId);

	WRITE_LOCK;
	{
		if (objType == GameObjectType_PLAYER)
		{
			shared_ptr<Player> player = nullptr;

			auto iter = _players.find(objectId);
			if (iter != _players.end())
			{
				player = iter->second;
				return player;
			}
		}
	}

	return nullptr;
}

int32 ObjectManager::GenerateId(GameObjectType type)
{
	WRITE_LOCK;
	{
		// 타입을 자리에 맞게 밀어 넣고 아이디를 넣어줌
		return (static_cast<int32>(type) << 24) | (_counter++);
	}
}

GameObjectType ObjectManager::GetObjectTypeById(int32 id)
{
	// 아이디에 해당하는 24 비트만큼 오른쪽 시프트하고 0111 1111로 비트마스크해서 타입 얻어옴
	int32 type = (id >> 24) & 0x7F/*127의 16진수*/;

	return static_cast<GameObjectType>(type);
}
