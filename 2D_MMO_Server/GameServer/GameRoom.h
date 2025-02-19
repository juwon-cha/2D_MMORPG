#pragma once
#include "pch.h"

class Player;

class GameRoom : public enable_shared_from_this<GameRoom>
{
public:
	int32 GetRoomId() const;
	void SetRoomId(int32 roomId);
	void EnterGame(shared_ptr<Player> newPlayer);
	void LeaveGame(int32 playerId);
	void Broadcast(SendBufferRef buffer);
	void HandleMove(shared_ptr<Player> player, const C_MOVE* movePkt);

private:
	USE_LOCK;
	int32 _roomId;
	map<uint32, shared_ptr<Player>> _players;
};

