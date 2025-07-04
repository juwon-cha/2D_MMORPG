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

shared_ptr<GameRoom> RoomManager::Add(int32 mapId)
{
	shared_ptr<GameRoom> gameRoom = make_shared<GameRoom>();
	gameRoom->Init(mapId);

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

void RoomManager::UpdateAllRooms()
{
	for (auto& pair : _rooms)
	{
		pair.second->Update(); // 각 방의 실시간 게임 로직 업데이트
	}
}

void RoomManager::FlushAllRoomJobs()
{
	for (auto& pair : _rooms)
	{
		pair.second->FlushJob(); // 각 방의 Job Queue 처리
	}
}
