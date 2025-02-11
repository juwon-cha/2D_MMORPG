#include "pch.h"
#include "Manager.h"
#include "Listener.h"
#include "ThreadPool.h"
#include "PoolManager.h"
#include "Service.h"
#include <fstream>

PacketManager& Manager::Packet = PacketManager::Instance();
DbManager& Manager::DB = DbManager::Instance();

ServerSession* Manager::session = nullptr;
shared_ptr<ServerService> service;
void Manager::Init()
{
	// -------------- DBManager Init ----------------
	{
		string connectionDataPath = COMMON_JSON_PATH + (string)"DBConnectionData.json";

		ifstream connection(connectionDataPath);
		bool s = connection.is_open();
		ASSERT(connection.is_open(), "");

		json j = json::parse(connection);

		ASSERT((j.find("password") != j.end()), "");
		ASSERT((j.find("uid") != j.end()), "");
		ASSERT((j.find("db_name") != j.end()), "");
		ASSERT((j.find("ip") != j.end()), "");
		ASSERT((j.find("driver") != j.end()), "");
		Manager::DB.Init(j);
		connection.close();
	}
	// -------------- DBManager Init ----------------

	// -------------- GameServer Connecting ---------
	{
		auto serverSession = GPoolManager->Pop<ServerSession>();
		Manager::session = serverSession;
		GPoolManager->Push<ServerSession>(serverSession);

		service = std::make_shared<ServerService>(
			NetAddress(L"127.0.0.1", 8001),
			std::make_shared<IocpCore>(),
			[]() { return shared_ptr<ServerSession>(GPoolManager->Pop<ServerSession>()); }, // TODO : SessionManager
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

		for (int32 i = 0; i < 3; i++)
			GThreadManager->EnqueueJob([]() { service->GetIocpCore()->Dispatch(); });


		/*string portJsonPath = COMMON_JSON_PATH + (string)"port.json";
		ifstream port(portJsonPath);

		ASSERT(port.is_open(), "");*/

		//static shared_ptr<ServerService> service = make_shared<ServerService>(
		//	NetAddress(L"127.0.0.1", 8080),
		//	make_shared<IocpCore>(),
		//	[]() { return shared_ptr<ServerSession>(Manager::session); }, // TODO : SessionManager µî
		//	1);
		//ASSERT(service->Start(), "SERVICE_START_ERROR");

		//json j = json::parse(port);
		//port.close();

		//sockaddr_in addr;
		//::memset(&addr, 0, sizeof(addr));
		//addr.sin_addr.s_addr = htonl(INADDR_ANY);
		//addr.sin_family = AF_INET;
		//addr.sin_port = htons(::atoi(j["db_to_server"].dump().c_str()));

	}
	// -------------- GameServer Connecting ---------
}