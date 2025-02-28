#pragma once
#include "GameObject.h"

class Player;

class ObjectManager
{
public:
	static ObjectManager& Instance();

	ObjectManager(const ObjectManager&) = delete;
	ObjectManager& operator=(const ObjectManager&) = delete;

public:
	bool Remove(int32 objectId);
	shared_ptr<GameObject> Find(int32 objectId);
	int32 GenerateId(ObjectType type);
	static ObjectType GetObjectTypeById(int32 id);

	template<typename T>
	shared_ptr<T> Add()
	{
		shared_ptr<T> gameObj = make_shared<T>();
		if (gameObj == nullptr)
		{
			return nullptr;
		}

		gameObj->SetObjectId(GenerateId(gameObj->GetObjectType()));

		WRITE_LOCK;
		{
			if (gameObj->GetObjectType() == ObjectType::PLAYER)
			{
				shared_ptr<GameObject> temp = static_pointer_cast<GameObject>(gameObj);
				_players.insert(pair<int32, shared_ptr<Player>>(gameObj->GetObjectId(), static_pointer_cast<Player>(temp)));
			}
		}

		return gameObj;
	}

private:
	ObjectManager() = default;
	~ObjectManager() = default;

	static ObjectManager* _instance;

private:
	USE_LOCK;
	map<int32, shared_ptr<Player>> _players;

	// [MSB(1)][OBJTYPE(7)][ID(24)]
	// [........|........|........|........]
	int32 _counter = 0;
};
