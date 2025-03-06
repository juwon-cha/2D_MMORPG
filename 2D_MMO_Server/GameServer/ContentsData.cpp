#include "pch.h"
#include "ContentsData.h"
#include <fstream>
#include <nlohmann/json.hpp>

map<uint32, Stat> StatData::MakeData()
{
    map<uint32, Stat> statData;

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
        stat.Level = iter->at("Level").get<uint32>();
        stat.MaxHp = iter->at("MaxHp").get<uint32>();
        stat.Attack = iter->at("Attack").get<uint32>();
        stat.TotalExp = iter->at("TotalExp").get<uint32>();
        statData._stats.push_back(stat);
    }

    return statData;
}

map<uint32, Skill> SkillData::MakeData()
{
    map<uint32, Skill> skillData;

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
        skill.ID = iter->at("ID").get<uint32>();
        skill.Name = iter->at("Name").get<string>();
        skill.CoolTime = iter->at("CoolTime").get<float>();
        skill.Damage = iter->at("Damage").get<uint32>();
        skill.SkillType = iter->at("SkillType").get<SkillType>();

        if (iter->contains("Projectile"))
        {
            auto projectile = iter->find("Projectile");
            skill.Projectile.Name = projectile->at("Name").get<string>();
            skill.Projectile.Speed = projectile->at("Speed").get<float>();
            skill.Projectile.Range = projectile->at("Range").get<uint32>();
            skill.Projectile.Prefab = projectile->at("Prefab").get<string>();
        }

        skillData._skills.push_back(skill);
    }

    return skillData;
}
