#pragma once
#include "pch.h"

class GameServerSession;

class SessionManager
{
public:
	void Add(shared_ptr<GameServerSession> session);
	void Remove(shared_ptr<GameServerSession> session);
	void Broadcast(SendBufferRef buffer);

private:
	USE_LOCK;
	set<shared_ptr<GameServerSession>> _sessions;
};

extern SessionManager GSessionManager;