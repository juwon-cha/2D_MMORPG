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

void ClientSessionManager::Broadcast(BYTE* buffer, int32 size)
{
	WRITE_LOCK;
	for (shared_ptr<ClientSession> session : _sessions)
	{
		char* temp = (char*)buffer;
		session->Send(buffer, size);
	}
}