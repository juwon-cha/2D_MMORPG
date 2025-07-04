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

std::vector<shared_ptr<ClientSession>> ClientSessionManager::GetSessions()
{
	WRITE_LOCK;

	// _sessions set의 내용을 그대로 복사
	// std::vector는 std::set의 반복자를 사용하여 생성
	std::vector<shared_ptr<ClientSession>> sessionsVec(_sessions.begin(), _sessions.end());

	return sessionsVec;
}
