#include "pch.h"
#include "Manager.h"
#include "Listener.h"
#include "ThreadPool.h"
#include "PoolManager.h"
#include <fstream>

PacketManager& Manager::Packet = PacketManager::Instance();
DbManager& Manager::DB = DbManager::Instance();

ServerSession* Manager::session = nullptr;
shared_ptr<ServerService> Manager::service = nullptr;
void Manager::Init()
{
	// -------------- DBManager Init ----------------
	{
		string connectionDataPath = COMMON_JSON_PATH + (string)"DBConnectionData.json";

		ifstream connection(connectionDataPath);
		bool s = connection.is_open();
		CRASH_ASSERT(connection.is_open(), "");

		json j = json::parse(connection);

		CRASH_ASSERT((j.find("password") != j.end()), "");
		CRASH_ASSERT((j.find("uid") != j.end()), "");
		CRASH_ASSERT((j.find("db_name") != j.end()), "");
		CRASH_ASSERT((j.find("ip") != j.end()), "");
		CRASH_ASSERT((j.find("driver") != j.end()), "");
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
		CRASH_ASSERT(service->Start(), "SERVICE_START_ERROR");

		cout << "DBServer Listening..." << endl;

		for (int32 i = 0; i < 3; i++)
			GThreadManager->EnqueueJob([]() {
			while (true)
				service->GetIocpCore()->Dispatch();
				});
	}
	// -------------- GameServer Connecting ---------
}
