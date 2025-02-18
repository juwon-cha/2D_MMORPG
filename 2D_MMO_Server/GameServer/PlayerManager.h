#pragma once

class Player;

class PlayerManager
{
public:
	static PlayerManager& Instance();

	PlayerManager(const PlayerManager&) = delete;
	PlayerManager& operator=(const PlayerManager&) = delete;

public:
	shared_ptr<Player> Add();
	bool Remove(uint32 playerId);
	shared_ptr<Player> Find(uint32 playerId);

private:
	PlayerManager() = default;
	~PlayerManager() = default;

	static PlayerManager* _instance;

private:
	USE_LOCK;
	map<uint32, shared_ptr<Player>> _players;
	uint32 _playerId = 1;
};

