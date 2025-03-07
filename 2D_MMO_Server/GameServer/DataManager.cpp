#include "pch.h"
#include "DataManager.h"
#include "ContentsData.h"

map<uint32, Stat> DataManager::Stats;
map<uint32, Skill> DataManager::Skills;

void DataManager::LoadData()
{
	auto loadedStats = LoadJson<StatData>("StatData").MakeData();
	auto loadedSkills = LoadJson<SkillData>("SkillData").MakeData();
	Stats.insert(loadedStats.begin(), loadedStats.end());
	Skills.insert(loadedSkills.begin(), loadedSkills.end());
}
