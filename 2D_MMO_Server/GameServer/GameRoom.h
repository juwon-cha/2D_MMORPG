#pragma once
#include "pch.h"

class Map;
class GameObject;
class Player;
class Monster;

class GameRoom : public enable_shared_from_this<GameRoom>
{
public:
	GameRoom();
	~GameRoom() = default;

public:
	void Init(int32 mapId);
	void EnterGame(shared_ptr<GameObject> gameObj);
	void LeaveGame(int32 objectId);
	void HandleMove(shared_ptr<Player> player, const C_MOVE* movePkt);
	void HandleSkill(shared_ptr<Player> player, const C_SKILL* skillPkt);
	void Broadcast(SendBufferRef buffer);

public:
	int32 GetRoomId() const;
	void SetRoomId(int32 roomId);

private:
	USE_LOCK;
	int32 _roomId;
	map<uint32, shared_ptr<Player>> _players;
	map<uint32, shared_ptr<Monster>> _monsters;
	shared_ptr<Map> _map;
};
