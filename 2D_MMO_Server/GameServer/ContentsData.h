#pragma once
#include "DataManager.h"

#pragma region Stat
class Stat
{
public:
	uint32 Level = 0;
	uint32 MaxHp = 0;
	uint32 Attack = 0;
	uint32 TotalExp = 0;
};

class StatData : public ILoader<uint32, Stat>
{
public:
	map<uint32, Stat> MakeData() override;

	static StatData fromJson(const json& json);

private:
	vector<Stat> _stats;
};
#pragma endregion

#pragma region Skill
enum SkillType
{
	SKILL_NONE,
	SKILL_AUTO,
	SKILL_PROJECTILE
};

NLOHMANN_JSON_SERIALIZE_ENUM(SkillType, {
	{SKILL_NONE, nullptr},
	{SKILL_AUTO, "SkillAuto"},
	{SKILL_PROJECTILE, "SkillProjectile"}
	})

	class ProjectileInfo
{
public:
	string Name;
	float Speed = 0;
	uint32 Range = 0;
	string Prefab;
};

class Skill
{
public:
	uint32 ID = 0;
	string Name;
	float CoolTime = 0;
	uint32 Damage = 0;
	SkillType SkillType = SkillType::SKILL_NONE;
	ProjectileInfo Projectile;
};

class SkillData : public ILoader<uint32, Skill>
{
public:
	map<uint32, Skill> MakeData() override;

	static SkillData fromJson(const json& json);

private:
	vector<Skill> _skills;
};
#pragma endregion
