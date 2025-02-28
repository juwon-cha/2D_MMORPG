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
	// �ٸ� �÷��̾�鿡�� ���� ����
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
	uint64 moveTick = static_cast<uint64>((1000 / speed/*speed ���Ŀ� �������� ����*/));

	_nextMoveTick = GetTickCount64() + moveTick;

	Vector2Int monsterCellPos = Vector2Int(_posX, _posY);
	Vector2Int targetCellPos = _destPos;

	vector<Vector2Int> path = _room->GetMap()->FindPathBFS(monsterCellPos, targetCellPos, /*temp*/true);
	// �� �� �ִ� ��ΰ� ���ų� ������ ������ ����� �̵� ����
	if (path.size() < 2 || path.size() > _chaseCellDistance)
	{
		_target = nullptr;
		SetObjectState(ObjectState_IDLE);
		BroadcastMove();
		return;
	}

	// �̵�
	SetObjectMoveDir(GetDirFromVector(path[1] - monsterCellPos)); // �÷��̾ �ٶ󺸴� ���� ����
	_room->GetMap()->ApplyMove(shared_from_this(), path[1]/*���� ������ ��ǥ*/);

	BroadcastMove();
}

void Monster::UpdateSkill()
{
	// TODO: Hit
	if (_skillCoolTick == 0)
	{
		// Ÿ���� �ִ��� üũ
		if (_target == nullptr || _target->GetGameRoom() != _room)
		{
			_target = nullptr;
			SetObjectState(ObjectState_MOVING);
			BroadcastMove();
			return;
		}

		// ��ų ��� ������ �ð����� üũ
		Vector2Int monsterCellPos = Vector2Int(_posX, _posY);
		Vector2Int targetCellPos = Vector2Int(_target->GetObjectPosX(), _target->GetObjectPosY());

		Vector2Int dir = targetCellPos - monsterCellPos; // ���� ����
		int32 distance = dir.CellDistance();
		bool canUseSkill = (distance <= _skillRange && (dir.X == 0 || dir.Y == 0));
		if (canUseSkill == false)
		{
			SetObjectState(ObjectState_MOVING);
			BroadcastMove();
			return;
		}

		// Ÿ���� ���� �ֽ�
		MoveDir lookDir = GetDirFromVector(dir);
		if (_moveDir != lookDir) // ���� ������ Ÿ���� ������ �ƴϸ� Ÿ������ ���ϵ��� ���� ����
		{
			SetObjectMoveDir(lookDir);
			BroadcastMove();
		}

		// ������
		cout << _target->GetObjectName() << " was hit by " << _name << "!" << endl;

		// ��ų ��� Broadcast
		SetObjectInfo(_id, _name, _posX, _posY, ObjectState_SKILL, _moveDir);

		flatbuffers::FlatBufferBuilder builder;
		int32 skillId = 1; // TEMP
		auto skillInfo = CreateSkillInfo(builder, skillId);

		// �ٸ� �÷��̾�鿡�� �˷���
		auto skill = CreateSC_SKILL(builder, _id, skillInfo);
		auto respondSkillPkt = PacketManager::Instance().CreatePacket(skill, builder, PacketType_SC_SKILL);

		_room->Broadcast(respondSkillPkt);

		// ��ų ��Ÿ�� ����
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
