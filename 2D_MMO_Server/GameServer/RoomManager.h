#pragma once

class GameRoom;

class RoomManager
{
public:
	static RoomManager& Instance();

	RoomManager(const RoomManager&) = delete;
	RoomManager& operator=(const RoomManager&) = delete;

public:
	shared_ptr<GameRoom> Add();
	bool Remove(uint32 roomId);
	shared_ptr<GameRoom> Find(uint32 roomId);

private:
	RoomManager() = default;
	~RoomManager() = default;

	static RoomManager* _instance;

private:
	USE_LOCK;
	map<uint32, shared_ptr<GameRoom>> _rooms;
	uint32 _roomId = 1;
};

