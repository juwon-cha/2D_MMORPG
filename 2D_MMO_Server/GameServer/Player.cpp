#include "pch.h"
#include "Player.h"

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

int32 Player::GetPlayerId() const
{
	return _id;
}

std::string Player::GetPlayerName() const
{
	return _name;
}

int32 Player::GetPlayerPosX() const
{
	return _posX;
}

int32 Player::GetPlayerPosY() const
{
	return _posY;
}

ObjectState Player::GetPlayerState() const
{
	return _state;
}

MoveDir Player::GetPlayerMoveDir() const
{
	return _moveDir;
}

void Player::SetPlayerId(int32 id)
{
	_id = id;
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

