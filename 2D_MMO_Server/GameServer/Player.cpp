#include "pch.h"
#include "Player.h"
#include "ContentsData.h"
#include "GameRoom.h"
#include "ItemManager.h"

Player::Player()
	: _session(nullptr)
	, _mapId(1)
{
	_type = GameObjectType_PLAYER;
	_inventory = make_shared<ItemManager>();
}

void Player::OnDamaged(shared_ptr<GameObject> attacker, int32 damage)
{
	GameObject::OnDamaged(attacker, damage);

	cout << "Damaged " << damage << "! From " << attacker->GetObjectName() << endl;
}

void Player::OnDead(shared_ptr<GameObject> attacker)
{
	GameObject::OnDead(attacker);

	shared_ptr<GameRoom> room = _room;
	room->LeaveGame(_id);

	// 플레이어 정보 리셋
	SetObjectInfo(_id, "Player " + to_string(_id));
	_hp = _maxHp;
	SetPosInfo(0, 0, ObjectState_IDLE, MoveDir_DOWN);
	SetStatInfo(_level, _speed, _hp, _maxHp, _attack, _totalExp);

	room->EnterGame(shared_from_this());
}
