#pragma once
#include "pch.h"

class Map;
class GameObject;
class Player;
class Monster;
class Projectile;

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
	shared_ptr<Player> FindPlayer(function<bool(shared_ptr<GameObject>)> condition);
	void Broadcast(SendBufferRef buffer);
	void Update();

public:
	int32 GetRoomId() const { return _roomId; }
	void SetRoomId(int32 roomId) { _roomId = roomId; }

	shared_ptr<Map> GetMap() const { return _map; }

private:
	USE_LOCK;
	int32 _roomId;
	map<int32, shared_ptr<Player>> _players;
	map<int32, shared_ptr<Monster>> _monsters;
	map<int32, shared_ptr<Projectile>> _projectiles;
	shared_ptr<Map> _map;
};
