#pragma once

#include "DBSession.h"

class ClientSession;

class ClientSessionManager
{
public:
	shared_ptr<DBSession> DB = make_shared<DBSession>();
	uint64 _id = 0;

public:
	void Add(shared_ptr<ClientSession> session);
	void Remove(shared_ptr<ClientSession> session);
	void Broadcast(shared_ptr<SendBuffer> buffer);
	shared_ptr<ClientSession> Find(uint64 id);

private:
	USE_LOCK;
	set<shared_ptr<ClientSession>> _sessions;
};

extern ClientSessionManager GSessionManager;