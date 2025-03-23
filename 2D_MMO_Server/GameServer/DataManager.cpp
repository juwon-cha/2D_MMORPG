#include "pch.h"
#include "DataManager.h"
#include "ContentsData.h"

map<int32, Stat> DataManager::Stats;
map<int32, Skill> DataManager::Skills;
map<int32, shared_ptr<ItemData>> DataManager::Items;

void DataManager::LoadData()
{
	auto loadedStats = LoadJson<StatData>("StatData").MakeData();
	auto loadedSkills = LoadJson<SkillData>("SkillData").MakeData();
	auto loadedItems = LoadJson<ItemLoader>("ItemData").MakeData();
	Stats.insert(loadedStats.begin(), loadedStats.end());
	Skills.insert(loadedSkills.begin(), loadedSkills.end());
	Items.insert(loadedItems.begin(), loadedItems.end());
}
