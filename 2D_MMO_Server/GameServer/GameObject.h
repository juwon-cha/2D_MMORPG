#pragma once
#include "Map.h"

class GameRoom;

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	virtual ~GameObject() = default;

public:
	Vector2Int GetFrontCellPos(MoveDir dir);
	Vector2Int GetFrontCellPos();
	static MoveDir GetDirFromVector(Vector2Int vector);
	virtual void Update();
	virtual void OnDamaged(shared_ptr<GameObject> attacker, int32 damage);
	virtual void OnDead(shared_ptr<GameObject> attacker);

public:
	GameObjectType GetObjectType() const { return _type; }

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
	void SetObjectMoveDir(MoveDir moveDir) { _moveDir = moveDir; }

	int32 GetObjectLevel() const { return _level; }
	int32 GetObjectHP() const { return _hp; }
	int32 GetObjectMaxHP() const { return _maxHp; }
	int32 GetObjectAttack() const { return _attack; }
	int32 GetObjectTotalExp() const { return _totalExp; }
	float GetObjectSpeed() const { return _speed; }
	void SetObjectSpeed(float speed) { _speed = speed; }

	void SetObjectInfo(int32 id, std::string name);
	void SetPosInfo(int32 posX, int32 posY, ObjectState state, MoveDir moveDir);
	void SetStatInfo(int32 level, float speed, int32 hp, int32 maxHp, int32 attack, int32 totalExp);

	shared_ptr<GameRoom> GetGameRoom() const { return _room; }
	void SetGameRoom(shared_ptr<GameRoom> room) { _room = room; }

protected:
	GameObjectType _type;
	int32 _id;
	int32 _posX;
	int32 _posY;
	std::string _name;
	ObjectState _state;
	MoveDir _moveDir;
	Vector2Int _cellPos;

	int32 _level;
	float _speed;
	int32 _hp;
	int32 _maxHp;
	int32 _attack;
	int32 _totalExp;

	shared_ptr<GameRoom> _room;
};
