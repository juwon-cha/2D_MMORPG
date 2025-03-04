#pragma once

class ServerConfig
{
public:
	ServerConfig() = default;
	~ServerConfig() = default;

public:
	static ServerConfig fromJson(const json& json);

public:
	string GetIpAddr() const { return _ipAddr; }
	string GetPort() const { return _port; }
	string GetdataPath() const { return _dataPath; }

private:
	string _ipAddr;
	string _port;
	string _dataPath;
};

class ConfigManager
{
public:
	ConfigManager() = default;
	~ConfigManager() = default;

public:
	static ConfigManager LoadConfig();

public:
	ServerConfig GetServerConfig() const { return _config; }
	void SetServerConfig(ServerConfig config) { _config = config; }

private:
	ServerConfig _config;
};
