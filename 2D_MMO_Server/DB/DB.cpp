#include "pch.h"
#include "ThreadPool.h"
#include "Service.h"
#include "GameServerSession.h"
#include "SessionManager.h"

int main()
{
	shared_ptr<ServerService> service = std::make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 8001),
		std::make_shared<IocpCore>(),
		[]() { return std::make_shared<GameServerSession>(); }, // TODO : SessionManager
		1);

	ASSERT(service->Start(), "SERVICE_START_ERROR");

	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->EnqueueJob([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}
	cout << "DBServer Listening..." << endl;

	while(true){ }
}