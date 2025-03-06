#pragma once
#include "pch.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "ConfigManager.h"

class Stat;
class Skill;

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
	static map<uint32, Stat> Stats;
	static map<uint32, Skill> Skills;
};
