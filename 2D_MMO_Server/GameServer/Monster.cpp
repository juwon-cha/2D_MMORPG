#include "pch.h"
#include <random>
#include "Monster.h"
#include "GameRoom.h"
#include "Player.h"
#include "PacketManager.h"

Monster::Monster()
	: _nextPatrolTick(0)
	, _nextSearchTick(0)
	, _nextMoveTick(0)
	, _skillCoolTick(0)
	, _searchCellDistance(0)
	, _chaseCellDistance(0)
	, _skillRange(0)
{
	_type = ObjectType::MONSTER;

	// TEMP
	_state = ObjectState_IDLE;

	_searchCellDistance = static_cast<int32>(CellDistance::DEFAULT_SEARCH_DIST);
	_chaseCellDistance = static_cast<int32>(CellDistance::DEFAULT_CHASE_DIST);
	_skillRange = static_cast<int32>(CellDistance::DEFAULT_SKILL_RANGE);
}

// Finite State Machine
void Monster::Update()
{
	switch (_state)
	{
	case ObjectState_IDLE:
		UpdateIdle();
		break;

	case ObjectState_MOVING:
		UpdateMoving();
		break;

	case ObjectState_SKILL:
		UpdateSkill();
		break;

	case ObjectState_DEAD:
		UpdateDead();
		break;
	}
}

void Monster::BroadcastMove()
{
	// 다른 플레이어들에게 정보 전송
	flatbuffers::FlatBufferBuilder builder;
	auto posInfo = CreatePositionInfo(builder, _state, _moveDir, _posX, _posY);
	auto move = CreateSC_MOVE(builder, _id, posInfo);
	auto movePkt = PacketManager::Instance().CreatePacket(move, builder, PacketType_SC_MOVE);

	_room->Broadcast(movePkt);
}

void Monster::UpdateIdle()
{
	Patrol();

}

void Monster::UpdateMoving()
{
	if (_nextMoveTick > GetTickCount64())
	{
		return;
	}

	float speed = 3.0f;
	uint64 moveTick = static_cast<uint64>((1000 / speed/*speed 추후에 스탯으로 관리*/));

	_nextMoveTick = GetTickCount64() + moveTick;

	Vector2Int monsterCellPos = Vector2Int(_posX, _posY);
	Vector2Int targetCellPos = _destPos;

	vector<Vector2Int> path = _room->GetMap()->FindPathBFS(monsterCellPos, targetCellPos, /*temp*/true);
	// 갈 수 있는 경로가 없거나 추적할 범위를 벗어나면 이동 멈춤
	if (path.size() < 2 || path.size() > _chaseCellDistance)
	{
		_target = nullptr;
		SetObjectState(ObjectState_IDLE);
		BroadcastMove();
		return;
	}

	// 이동
	SetObjectMoveDir(GetDirFromVector(path[1] - monsterCellPos)); // 플레이어를 바라보는 방향 벡터
	_room->GetMap()->ApplyMove(shared_from_this(), path[1]/*다음 가야할 좌표*/);

	BroadcastMove();
}

void Monster::UpdateSkill()
{
	// TODO: Hit
	if (_skillCoolTick == 0)
	{
		// 타겟이 있는지 체크
		if (_target == nullptr || _target->GetGameRoom() != _room)
		{
			_target = nullptr;
			SetObjectState(ObjectState_MOVING);
			BroadcastMove();
			return;
		}

		// 스킬 사용 가능한 시간인지 체크
		Vector2Int monsterCellPos = Vector2Int(_posX, _posY);
		Vector2Int targetCellPos = Vector2Int(_target->GetObjectPosX(), _target->GetObjectPosY());

		Vector2Int dir = targetCellPos - monsterCellPos; // 방향 벡터
		int32 distance = dir.CellDistance();
		bool canUseSkill = (distance <= _skillRange && (dir.X == 0 || dir.Y == 0));
		if (canUseSkill == false)
		{
			SetObjectState(ObjectState_MOVING);
			BroadcastMove();
			return;
		}

		// 타겟팅 방향 주시
		MoveDir lookDir = GetDirFromVector(dir);
		if (_moveDir != lookDir) // 현재 방향이 타겟팅 방향이 아니면 타겟팅을 향하도록 방향 변경
		{
			SetObjectMoveDir(lookDir);
			BroadcastMove();
		}

		// 데미지
		cout << _target->GetObjectName() << " was hit by " << _name << "!" << endl;

		// 스킬 사용 Broadcast
		SetObjectInfo(_id, _name, _posX, _posY, ObjectState_SKILL, _moveDir);

		flatbuffers::FlatBufferBuilder builder;
		int32 skillId = 1; // TEMP
		auto skillInfo = CreateSkillInfo(builder, skillId);

		// 다른 플레이어들에게 알려줌
		auto skill = CreateSC_SKILL(builder, _id, skillInfo);
		auto respondSkillPkt = PacketManager::Instance().CreatePacket(skill, builder, PacketType_SC_SKILL);

		_room->Broadcast(respondSkillPkt);

		// 스킬 쿨타임 적용
		uint64 coolTick = 1000; // TEMP
		_skillCoolTick = GetTickCount64() + coolTick;
	}

	if (_skillCoolTick > GetTickCount64())
	{
		return;
	}

	_skillCoolTick = 0;
}

void Monster::UpdateDead()
{
}

void Monster::Patrol()
{
	if (_nextPatrolTick > GetTickCount64())
	{
		return;
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	//std::uniform_int_distribution<uint64> dis(1, 4);
	_nextPatrolTick = GetTickCount64() + 1000;

	Vector2Int monsterCellPos = Vector2Int(_posX, _posY);

	std::uniform_int_distribution<int32> randCoordinates(-5, 5);
	int32 x = randCoordinates(gen);
	int32 y = randCoordinates(gen);

	Vector2Int randomPos = monsterCellPos + Vector2Int(x, y);

	if (_room->GetMap()->CanGo(randomPos) && _room->GetMap()->Find(randomPos) == nullptr)
	{
		_destPos = randomPos;
		SetObjectState(ObjectState_MOVING);
	}
}
