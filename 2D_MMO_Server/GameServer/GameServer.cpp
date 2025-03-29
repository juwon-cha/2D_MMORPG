#include "pch.h"
#include "ThreadPool.h"
#include "Service.h"
#include "Session.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "DBSession.h"
#include "RoomManager.h"
#include "GameRoom.h"
#include "ConfigManager.h"
#include "StringConverter.h"
#include "DataManager.h"
#include "ClientSessionManager.h"

int main()
{
#pragma region DB 서버 연결
	shared_ptr<ClientService> dbConnect = std::make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 8001),
		std::make_shared<IocpCore>(),
		[]() {return GSessionManager.DB; },
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
#pragma endregion

	// GameServer Listening
	ConfigManager config = ConfigManager::LoadConfig();
	DataManager::LoadData();

	// 전체 맵 로드
	for (int32 i = 1; i <= 5; ++i)
	{
		RoomManager::Instance().Add(i);
	}

	wstring ipAddr = StringConverter::ConvertStringToWString(config.GetServerConfig().GetIpAddr().c_str(), config.GetServerConfig().GetIpAddr().size());
	uint16 portNum = static_cast<uint16>(stoi(config.GetServerConfig().GetPort()));

	shared_ptr<ServerService> service = std::make_shared<ServerService>(
		NetAddress(ipAddr, portNum),
		std::make_shared<IocpCore>(),
		std::make_shared<ClientSession>,
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

	while (true)
	{
		// 전체 룸 업데이트
		for (int32 i = 1; i <= 5; ++i)
		{
			RoomManager::Instance().Find(i)->Update();
		}

		this_thread::sleep_for(100ms);
	}
}