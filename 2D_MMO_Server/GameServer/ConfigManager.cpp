#include "pch.h"
#include "ConfigManager.h"
#include <fstream>
#include <nlohmann/json.hpp>

ServerConfig ServerConfig::fromJson(const json& json)
{
	ServerConfig config;

	config._ipAddr = json.at("ipAddr").get<string>();
	config._port = json.at("port").get<string>();
	config._dataPath = json.at("dataPath").get<string>();

	return config;
}

ConfigManager ConfigManager::LoadConfig()
{
	ConfigManager configManager;

	ifstream f("../../Common/Data/config.json");
	json data = json::parse(f);
	ServerConfig serverConfig = ServerConfig::fromJson(data);

	configManager.SetServerConfig(serverConfig);

	return configManager;
}
