#include "pch.h"
#include "Player.h"
#include "ContentsData.h"
#include "DataManager.h"

Player::Player()
	: _session(nullptr)
{
	_type = ObjectType::PLAYER;

	// TEMP
	_hp = 100;
	_maxHp = 100;
	_speed = 10.0f;
}

void Player::OnDamaged(shared_ptr<GameObject> attacker, int32 damage)
{
	cout << "Damaged " << damage << "! From " << attacker->GetObjectName() << endl;
}
