#include "pch.h"
#include "Player.h"
#include "ContentsData.h"
#include "GameRoom.h"

Player::Player()
	: _session(nullptr)
{
	_type = ObjectType::PLAYER;
}

void Player::OnDamaged(shared_ptr<GameObject> attacker, int32 damage)
{
	GameObject::OnDamaged(attacker, damage);

}

void Player::OnDead(shared_ptr<GameObject> attacker)
{
	GameObject::OnDead(attacker);

	// 클라이언트에서 애니메이션 재생 후 리스폰
	this_thread::sleep_for(1.5s);

	shared_ptr<GameRoom> room = _room;
	room->LeaveGame(_id);

	// 플레이어 정보 리셋
	SetObjectInfo(_id, "Player " + to_string(_id));
	_hp = _maxHp;
	SetPosInfo(0, 0, ObjectState_IDLE, MoveDir_DOWN);
	SetStatInfo(_level, _speed, _hp, _maxHp, _attack, _totalExp);

	room->EnterGame(shared_from_this());
}
