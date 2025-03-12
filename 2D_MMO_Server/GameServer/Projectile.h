#pragma once
#include "GameObject.h"
#include "ContentsData.h"

class Projectile : public GameObject
{
public:
	Projectile();
	~Projectile() = default;

	void Update() override;

public:
	shared_ptr<GameObject> GetOwner() const { return _owner; }
	void SetOwner(shared_ptr<GameObject> owner) { _owner = owner; }

	Vector2Int GetOwnerPos() const { return _ownerPos; }
	void SetOwnerPos(int32 x, int32 y) { _ownerPos = Vector2Int(x, y); }

	Skill GetSkillData() const { return _skillData; }
	void SetSkillData(Skill skillData) { _skillData = skillData; }

private:
	shared_ptr<GameObject> _owner;
	Vector2Int _ownerPos; // 처음 투사체를 발사한 좌표
	Skill _skillData;
	uint64 _nextMoveTick;
};
