#include "pch.h"
#include "Player.h"

Player::Player() 
	: _id(0)
	, _posX(0)
	, _posY(0)
	, _state(ObjectState_IDLE)
	, _moveDir(MoveDir_NONE)
	, _cellPos(0, 0)
	, _room(nullptr)
	, _session(nullptr)
{
}

shared_ptr<GameRoom> Player::GetGameRoom() const
{
	return _room;
}

void Player::SetGameRoom(shared_ptr<GameRoom> room)
{
	_room = room;
}

shared_ptr<ClientSession> Player::GetClientSession() const
{
	return _session;
}

void Player::SetClientSession(shared_ptr<ClientSession> session)
{
	_session = session;
}

void Player::SetPlayerInfo(int32 id, std::string name, int32 posX, int32 posY, ObjectState state, MoveDir moveDir)
{
	_id = id;
	_name = name;
	_posX = posX;
	_posY = posY;
	_state = state;
	_moveDir = moveDir;
}

