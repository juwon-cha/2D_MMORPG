#pragma once
#include "pch.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "ConfigManager.h"

class Stat;
class Skill;
class ItemData;

template<typename Key, typename Value>
class ILoader
{
public:
	virtual map<Key, Value> MakeData() = 0;
};

class DataManager
{
public:
	static void LoadData();

private:
	template<typename Loader>
	static Loader LoadJson(string path)
	{
		ConfigManager config = ConfigManager::LoadConfig();

		string fileName = path + ".json";
		string filePath = config.GetServerConfig().GetDataPath() + "/" + fileName;
		ifstream f(filePath);

		json data = json::parse(f);
		return Loader::fromJson(data);
	}

public:
	static map<int32, Stat> Stats;
	static map<int32, Skill> Skills;
	static map<int32, shared_ptr<ItemData>> Items;
};
