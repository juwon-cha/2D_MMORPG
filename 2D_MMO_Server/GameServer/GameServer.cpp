#include "pch.h"
#include "ThreadPool.h"
#include "Service.h"
#include "Session.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "DBSession.h"

int main()
{
	shared_ptr<ClientService> dbConnect = std::make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 8001),
		std::make_shared<IocpCore>(),
		[]() { return std::make_shared<DBSession>(); },
		1);
	ASSERT(dbConnect->Start(), "SERVICE_START_ERROR");

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->EnqueueJob([=]()
			{
				while (true)
				{
					dbConnect->GetIocpCore()->Dispatch();
				}
			});
	}

	shared_ptr<ServerService> service = std::make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 8002),
		std::make_shared<IocpCore>(),
		[]() { return std::make_shared<ClientSession>(); }, // TODO : SessionManager
		1);

	ASSERT(service->Start(), "SERVICE_START_ERROR");

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->EnqueueJob([=]()
			{ 
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}
	cout << "GameServer Listening..." << endl;

	while (true){ }
}