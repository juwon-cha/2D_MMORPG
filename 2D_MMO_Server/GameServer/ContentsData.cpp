#include "pch.h"
#include "ContentsData.h"
#include <fstream>
#include <nlohmann/json.hpp>

map<int32, Stat> StatData::MakeData()
{
    map<int32, Stat> statData;

    for (Stat stat : _stats)
    {
        statData.insert(make_pair(stat.Level, stat));
    }

    return statData;
}

StatData StatData::fromJson(const json& json)
{
    StatData statData;
    Stat stat;

    for (auto iter = json["stats"].begin(); iter != json["stats"].end(); ++iter)
    {
        stat.Level = iter->at("Level").get<int32>();
        stat.Speed = iter->at("Speed").get<float>();
        stat.MaxHp = iter->at("MaxHp").get<int32>();
        stat.Hp = stat.MaxHp;
        stat.Attack = iter->at("Attack").get<int32>();
        stat.TotalExp = iter->at("TotalExp").get<int32>();
        statData._stats.push_back(stat);
    }

    return statData;
}

map<int32, Skill> SkillData::MakeData()
{
    map<int32, Skill> skillData;

    for (Skill skill : _skills)
    {
        skillData.insert(make_pair(skill.ID, skill));
    }

    return skillData;
}

SkillData SkillData::fromJson(const json& json)
{
    SkillData skillData;
    Skill skill;

    for (auto iter = json["skills"].begin(); iter != json["skills"].end(); ++iter)
    {
        skill.ID = iter->at("ID").get<int32>();
        skill.Name = iter->at("Name").get<string>();
        skill.CoolTime = iter->at("CoolTime").get<float>();
        skill.Damage = iter->at("Damage").get<int32>();
        skill.SkillType = iter->at("SkillType").get<SkillType>();

        if (iter->contains("Projectile"))
        {
            auto projectile = iter->find("Projectile");
            skill.Projectile.Name = projectile->at("Name").get<string>();
            skill.Projectile.Speed = projectile->at("Speed").get<float>();
            skill.Projectile.Range = projectile->at("Range").get<int32>();
            skill.Projectile.Prefab = projectile->at("Prefab").get<string>();
        }

        skillData._skills.push_back(skill);
    }

    return skillData;
}

map<int32, shared_ptr<ItemData>> ItemLoader::MakeData()
{
    map<int32, shared_ptr<ItemData>> itemData;

    for (shared_ptr<WeaponData> item : _weapons)
    {
        item->ItemType = ItemType_ITEM_WEAPON;
        itemData.insert(make_pair(item->ID, item));
    }

    for (shared_ptr<ArmorData> item : _armors)
    {
        item->ItemType = ItemType_ITEM_ARMOR;
        itemData.insert(make_pair(item->ID, item));
    }

    return itemData;
}

ItemLoader ItemLoader::fromJson(const json& json)
{
    ItemLoader itemLoader;

    for (auto iter = json["weapons"].begin(); iter != json["weapons"].end(); ++iter)
    {
        shared_ptr<WeaponData> weapon = make_shared<WeaponData>();

        weapon->ID = iter->at("ID").get<int32>();
        weapon->Name = iter->at("Name").get<string>();
        weapon->WeaponType = iter->at("WeaponType").get<WeaponType>();
        weapon->Damage = iter->at("Damage").get<int32>();

        itemLoader._weapons.push_back(weapon);
    }

    for (auto iter = json["armors"].begin(); iter != json["armors"].end(); ++iter)
    {
        shared_ptr<ArmorData> armor = make_shared<ArmorData>();

        armor->ID = iter->at("ID").get<int32>();
        armor->Name = iter->at("Name").get<string>();
        armor->Defence = iter->at("Defence").get<int32>();

        itemLoader._armors.push_back(armor);
    }

    return itemLoader;
}
