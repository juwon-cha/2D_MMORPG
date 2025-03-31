#include "pch.h"
#include "Projectile.h"
#include "GameRoom.h"
#include "GameObject.h"
#include "PacketManager.h"

Projectile::Projectile()
	: _owner(nullptr)
	, _nextMoveTick(0)
{
	_type = GameObjectType_PROJECTILE;
}

void Projectile::Update()
{
	if (_owner == nullptr || _room == nullptr)
	{
		return;
	}

	if (_nextMoveTick >= GetTickCount64())
	{
		return;
	}

	uint64 tick = static_cast<uint64>(1000 / _skillData.Projectile.Speed);
	_nextMoveTick = GetTickCount64() + tick;

	// 맵에서 갈 수 없는 곳이거나
	// 스킬 사용 범위를 넘어가면 투사체 사라짐
	Vector2Int projectileDist = Vector2Int(_posX, _posY);
	Vector2Int dir = projectileDist - _ownerPos; // 방향 벡터
	int32 skillDist = dir.CellDistance();

	Vector2Int destPos = GetFrontCellPos();
	if (_room->GetMap()->CanGo(destPos) && skillDist <= _skillData.Projectile.Range)
	{
		SetPosInfo(destPos.X, destPos.Y, _state, _moveDir);

		flatbuffers::FlatBufferBuilder builder;
		auto posInfo = CreatePositionInfo(builder, _state, _moveDir, _posX, _posY);
		auto move = CreateSC_MOVE(builder, _id, posInfo);
		auto movePkt = PacketManager::Instance().CreatePacket(move, builder, PacketType_SC_MOVE);

		_room->Broadcast(movePkt);

		cout << "Move Projectile" << endl;
	}
	else
	{
		shared_ptr<GameObject> target = _room->GetMap()->Find(destPos);
		if (target != nullptr)
		{
			// NON PVP
			if (target->GetObjectType() == GameObjectType_PLAYER)
			{
				return;
			}

			cout << "Arrow Hit!" << endl;
			target->OnDamaged(shared_from_this(), _skillData.Damage + _owner->GetObjectAttack());
		}

		// Destroy
		_room->LeaveGame(_id);
	}
}
