#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
	: _type(ObjectType::NONE)
	, _id(0)
	, _posX(0)
	, _posY(0)
	, _state(ObjectState_IDLE)
	, _moveDir(MoveDir_DOWN)
	, _cellPos(0, 0)
	, _hp(0)
	, _maxHp(0)
	, _speed(0)
	, _room(nullptr)
{
}

Vector2Int GameObject::GetFrontCellPos(MoveDir dir)
{
	Vector2Int cellPos = Vector2Int(_posX, _posY);

	switch (dir)
	{
	case MoveDir_UP:
		cellPos = cellPos + Vector2Int::Up();
		break;
	case MoveDir_DOWN:
		cellPos = cellPos + Vector2Int::Down();
		break;
	case MoveDir_LEFT:
		cellPos = cellPos + Vector2Int::Left();
		break;
	case MoveDir_RIGHT:
		cellPos = cellPos + Vector2Int::Right();
		break;
	}

	return cellPos;
}

Vector2Int GameObject::GetFrontCellPos()
{
	return GetFrontCellPos(_moveDir);
}

MoveDir GameObject::GetDirFromVector(Vector2Int vector)
{
	if (vector.X > 0)
	{
		return MoveDir_RIGHT;
	}
	else if (vector.X < 0)
	{
		return MoveDir_LEFT;
	}
	else if (vector.Y > 0)
	{
		return MoveDir_UP;
	}
	else
	{
		return MoveDir_DOWN;
	}
}

void GameObject::Update()
{
}

void GameObject::SetObjectInfo(int32 id, std::string name)
{
	_id = id;
	_name = name;
}

void GameObject::SetPosInfo(int32 posX, int32 posY, ObjectState state, MoveDir moveDir)
{
	_posX = posX;
	_posY = posY;
	_state = state;
	_moveDir = moveDir;
}

void GameObject::SetStatInfo(int32 hp, int32 maxHp, float speed)
{
	_hp = hp;
	_maxHp = maxHp;
	_speed = speed;
}
