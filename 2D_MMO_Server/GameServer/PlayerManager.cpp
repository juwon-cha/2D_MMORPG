#include "pch.h"
#include "PlayerManager.h"
#include "Player.h"

PlayerManager* PlayerManager::_instance = nullptr;

PlayerManager& PlayerManager::Instance()
{
	if (_instance == nullptr)
	{
		_instance = new PlayerManager();
	}

	return *_instance;
}

shared_ptr<Player> PlayerManager::Add()
{
	shared_ptr<Player> player = make_shared<Player>();

	WRITE_LOCK;
	player->SetPlayerId(_playerId);
	_players.insert(pair<uint32, shared_ptr<Player>>(_playerId, player));
	++_playerId;

	return player;
}

bool PlayerManager::Remove(uint32 playerId)
{
	WRITE_LOCK;
	return _players.erase(playerId);
}

shared_ptr<Player> PlayerManager::Find(uint32 playerId)
{
	shared_ptr<Player> room = nullptr;

	auto iter = _players.find(playerId);
	if (iter != _players.end())
	{
		room = iter->second;
		return room;
	}

	return nullptr;
}