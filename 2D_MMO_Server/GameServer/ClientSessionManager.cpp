#include "pch.h"
#include "ClientSessionManager.h"
#include "ClientSession.h"

ClientSessionManager GSessionManager;

void ClientSessionManager::Add(shared_ptr<ClientSession> session)
{
	WRITE_LOCK;
	session->SessionId = ++_id;
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

shared_ptr<ClientSession> ClientSessionManager::Find(uint64 id)
{
	shared_ptr<ClientSession> session = nullptr;

	{
		WRITE_LOCK;
		auto it = find_if(_sessions.begin(), _sessions.end(), [id](shared_ptr<ClientSession> client) { return client->SessionId == id; });
		if (it != _sessions.end())
			session = *it;
	}
	return session;
}
