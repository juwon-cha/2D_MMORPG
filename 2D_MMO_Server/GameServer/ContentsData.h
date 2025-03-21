#pragma once
#include "DataManager.h"

#pragma region Stat
class Stat
{
public:
	int32 Level = 0;
	float Speed = 0;
	int32 Hp = 0;
	int32 MaxHp = 0;
	int32 Attack = 0;
	int32 TotalExp = 0;
};

class StatData : public ILoader<int32, Stat>
{
public:
	map<int32, Stat> MakeData() override;

	static StatData fromJson(const json& json);

private:
	vector<Stat> _stats;
};
#pragma endregion

#pragma region Skill
NLOHMANN_JSON_SERIALIZE_ENUM(SkillType, {
	{SkillType_SKILL_NONE, nullptr},
	{SkillType_SKILL_AUTO, "SKILL_AUTO"},
	{SkillType_SKILL_PROJECTILE, "SKILL_PROJECTILE"}
	})

	class ProjectileInfo
{
public:
	string Name;
	float Speed = 0;
	int32 Range = 0;
	string Prefab;
};

class Skill
{
public:
	int32 ID = 0;
	string Name;
	float CoolTime = 0;
	int32 Damage = 0;
	SkillType SkillType = SkillType_SKILL_NONE;
	ProjectileInfo Projectile;
};

class SkillData : public ILoader<int32, Skill>
{
public:
	map<int32, Skill> MakeData() override;

	static SkillData fromJson(const json& json);

private:
	vector<Skill> _skills;
};
#pragma endregion

#pragma region Item
NLOHMANN_JSON_SERIALIZE_ENUM(ItemType, {
	{ItemType_ITEM_NONE, nullptr},
	{ItemType_ITEM_WEAPON, "ITEM_WEAPON"},
	{ItemType_ITEM_ARMOR, "ITEM_ARMOR"}
	});

NLOHMANN_JSON_SERIALIZE_ENUM(WeaponType, {
		{WeaponType_WEAPON_NONE, nullptr},
		{WeaponType_WEAPON_SWORD, "WEAPON_SWORD"},
		{WeaponType_WEAPON_BOW, "WEAPON_BOW"}
	});

class ItemData
{
public:
	int32 ID = 0;
	string Name;
	ItemType ItemType = ItemType_ITEM_NONE;
};

class WeaponData : public ItemData
{
public:
	WeaponType WeaponType = WeaponType_WEAPON_NONE;
	int32 Damage = 0;
};

class ArmorData : public ItemData
{
public:
	int32 Defence = 0;
};

class ItemLoader : public ILoader<int32, shared_ptr<ItemData>>
{
public:
	map<int32, shared_ptr<ItemData>> MakeData() override;

	static ItemLoader fromJson(const json& json);

private:
	vector<shared_ptr<WeaponData>> _weapons;
	vector<shared_ptr<ArmorData>> _armors;
};
#pragma endregion
