#include "SessionManager.h"
#include "GameServerSession.h"

SessionManager GSessionManager;

void SessionManager::Add(shared_ptr<GameServerSession> session)
{
	WRITE_LOCK;
	_sessions.insert(session);
}

void SessionManager::Remove(shared_ptr<GameServerSession> session)
{
	WRITE_LOCK;
	_sessions.erase(session);
}

void SessionManager::Broadcast(SendBufferRef buffer)
{
	WRITE_LOCK;
	for (shared_ptr<GameServerSession> session : _sessions)
	{
		session->Send(buffer);
	}
}
