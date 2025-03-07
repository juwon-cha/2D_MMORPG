#include "pch.h"
#include "Player.h"
#include "ContentsData.h"

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

}
