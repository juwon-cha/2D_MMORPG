#include "pch.h"
#include "RoomManager.h"
#include "GameRoom.h"

RoomManager* RoomManager::_instance = nullptr;

RoomManager& RoomManager::Instance()
{
	if (_instance == nullptr)
	{
		_instance = new RoomManager();
	}

	return *_instance;
}

shared_ptr<GameRoom> RoomManager::Add()
{
	shared_ptr<GameRoom> gameRoom = make_shared<GameRoom>();

	WRITE_LOCK;
	gameRoom->SetRoomId(_roomId);
	_rooms.insert(pair<uint32, shared_ptr<GameRoom>>(_roomId, gameRoom));
	++_roomId;

	return gameRoom;
}

bool RoomManager::Remove(uint32 roomId)
{
	WRITE_LOCK;
	return _rooms.erase(roomId);
}

shared_ptr<GameRoom> RoomManager::Find(uint32 roomId)
{
	shared_ptr<GameRoom> room = nullptr;

	auto iter = _rooms.find(roomId);
	if (iter != _rooms.end())
	{
		room = iter->second;
		return room;
	}

	return nullptr;
}
