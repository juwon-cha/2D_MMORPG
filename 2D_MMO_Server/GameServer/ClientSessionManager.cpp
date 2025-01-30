#include "pch.h"
#include "ClientSessionManager.h"
#include "ClientSession.h"

ClientSessionManager GSessionManager;

void ClientSessionManager::Add(shared_ptr<ClientSession> session)
{
	WRITE_LOCK;
	_sessions.insert(session);
}

void ClientSessionManager::Remove(shared_ptr<ClientSession> session)
{
	WRITE_LOCK;
	_sessions.erase(session);
}

void ClientSessionManager::Broadcast(shared_ptr<SendBuffer> buffer)
{
	WRITE_LOCK;
	for (shared_ptr<ClientSession> session : _sessions)
	{
		session->Send(buffer);
	}
}