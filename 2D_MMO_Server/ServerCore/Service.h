#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>

enum class ServiceType : uint8
{
	Server,
	Client
};

/*-------------
	Service
--------------*/

using SessionFactory = function<shared_ptr<Session>(void)>;

class COREDLL Service : public enable_shared_from_this<Service>
{
public:
	Service(ServiceType type, NetAddress address, shared_ptr<IocpCore> core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~Service();

	virtual bool		Start() abstract;
	bool				CanStart() { return _sessionFactory != nullptr; }

	virtual void		CloseService();
	void				SetSessionFactory(SessionFactory func) { _sessionFactory = func; }

	shared_ptr<Session>	CreateSession();
	void AddSession(shared_ptr<Session> session);
	void ReleaseSession(shared_ptr<Session> session);
	int32 GetCurrentSessionCount() { return _sessionCount; }
	int32 GetMaxSessionCount() { return _maxSessionCount; }

public:
	ServiceType GetServiceType() const { return _type; }
	NetAddress GetNetAddress() const { return _netAddress; }
	const shared_ptr<IocpCore>& GetIocpCore() const { return _iocpCore; }

protected:
	USE_LOCK;
	ServiceType	_type;
	NetAddress _netAddress = {};
	shared_ptr<IocpCore> _iocpCore;

	set<shared_ptr<Session>> _sessions;
	int32 _sessionCount = 0;
	int32 _maxSessionCount = 0;
	SessionFactory _sessionFactory;
};

/*-----------------
	ClientService
------------------*/

class COREDLL ClientService : public Service
{
public:
	ClientService(NetAddress targetAddress, shared_ptr<IocpCore> core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ClientService() {}

	virtual bool	Start() override;
};


/*-----------------
	ServerService
------------------*/

class COREDLL ServerService : public Service
{
public:
	ServerService(NetAddress targetAddress, shared_ptr<IocpCore> core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ServerService() {}

	virtual bool	Start() override;
	virtual void	CloseService() override;

private:
	shared_ptr<Listener> _listener = nullptr;
};