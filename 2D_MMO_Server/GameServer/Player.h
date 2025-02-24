#pragma once
#include "Map.h"

class GameRoom;
class ClientSession;

class Player
{
public:
	Player();
	~Player() = default;

public:
	int32 GetPlayerId() const { return _id; }
	void SetPlayerId(int32 id) { _id = id; }

	int32 GetPlayerPosX() const { return _posX; }
	void SetPlayerPosX(int32 x) { _posX = x; }
	int32 GetPlayerPosY() const { return _posY; }
	void SetPlayerPosY(int32 y) { _posY = y; }

	std::string GetPlayerName() const { return _name; }
	ObjectState GetPlayerState() const { return _state; }
	MoveDir GetPlayerMoveDir() const { return _moveDir; }


	void SetPlayerInfo(int32 id, std::string name, int32 posX, int32 posY, ObjectState state, MoveDir moveDir);

	shared_ptr<GameRoom> GetGameRoom() const;
	void SetGameRoom(shared_ptr<GameRoom> room);

	shared_ptr<ClientSession> GetClientSession() const;
	void SetClientSession(shared_ptr<ClientSession> session);

private:
	int32 _id;
	int32 _posX;
	int32 _posY;
	std::string _name;
	ObjectState _state;
	MoveDir _moveDir;
	Vector2Int _cellPos;

	shared_ptr<GameRoom> _room;
	shared_ptr<ClientSession> _session;
};
