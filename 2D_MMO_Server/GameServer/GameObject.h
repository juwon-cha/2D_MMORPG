#pragma once
#include "Map.h"

enum class ObjectType
{
	NONE,
	PLAYER,
	MONSTER,
	PROJECTILE
};

class GameRoom;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject() = default;

public:
	Vector2Int GetFrontCellPos(MoveDir dir);
	Vector2Int GetFrontCellPos();

public:
	ObjectType GetObjectType() const { return _type; }
	void SetObjectType(ObjectType type) { _type = type; }

	int32 GetObjectId() const { return _id; }
	void SetObjectId(int32 id) { _id = id; }

	int32 GetObjectPosX() const { return _posX; }
	void SetObjectPosX(int32 x) { _posX = x; }
	int32 GetObjectPosY() const { return _posY; }
	void SetObjectPosY(int32 y) { _posY = y; }

	std::string GetObjectName() const { return _name; }
	ObjectState GetObjectState() const { return _state; }
	void SetObjectState(ObjectState state) { _state = state; }
	MoveDir GetObjectMoveDir() const { return _moveDir; }

	void SetObjectInfo(int32 id, std::string name, int32 posX, int32 posY, ObjectState state, MoveDir moveDir);

	shared_ptr<GameRoom> GetGameRoom() const { return _room; }
	void SetGameRoom(shared_ptr<GameRoom> room) { _room = room; }

protected:
	ObjectType _type;
	int32 _id;
	int32 _posX;
	int32 _posY;
	std::string _name;
	ObjectState _state;
	MoveDir _moveDir;
	Vector2Int _cellPos;

	shared_ptr<GameRoom> _room;
};
