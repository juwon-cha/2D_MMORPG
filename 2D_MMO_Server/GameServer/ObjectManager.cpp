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
		// Ÿ���� �ڸ��� �°� �о� �ְ� ���̵� �־���
		return (static_cast<int32>(type) << 24) | (_counter++);
	}
}

GameObjectType ObjectManager::GetObjectTypeById(int32 id)
{
	// ���̵� �ش��ϴ� 24 ��Ʈ��ŭ ������ ����Ʈ�ϰ� 0111 1111�� ��Ʈ����ũ�ؼ� Ÿ�� ����
	int32 type = (id >> 24) & 0x7F/*127�� 16����*/;

	return static_cast<GameObjectType>(type);
}
