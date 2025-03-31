#pragma once
#include "GameObject.h"

class Player;

class Monster : public GameObject
{
public:
	Monster();
	virtual ~Monster() = default;

public:
	void Update() override;
	void OnDamaged(shared_ptr<GameObject> attacker, int32 damage) override;
	void OnDead(shared_ptr<GameObject> attacker) override;
	void BroadcastMove();

protected:
	void virtual UpdateIdle();
	void virtual UpdateMoving();
	void virtual UpdateSkill();
	void virtual UpdateDead();
	void virtual Patrol();
	void virtual SearchTarget();

private:
	enum class CellDistance
	{
		DEFAULT_SEARCH_DIST = 5,
		DEFAULT_CHASE_DIST = 10,
		DEFAULT_SKILL_RANGE = 1
	};

private:
	uint64 _nextPatrolTick;
	uint64 _nextSearchTick;
	uint64 _nextMoveTick;
	int32 _searchCellDistance;
	int32 _chaseCellDistance;
	int32 _skillRange;
	int32 _skillCoolTick;

	Vector2Int _destPos;
	shared_ptr<Player> _target;
};
