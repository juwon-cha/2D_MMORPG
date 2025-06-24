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

int main()
{
#pragma region DB
	shared_ptr<ClientService> dbConnect = std::make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 8001),
		std::make_shared<IocpCore>(),
		[]() {return GSessionManager.DB; },
		1);
	CRASH_ASSERT(dbConnect->Start(), "SERVICE_START_ERROR");
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

#pragma region GameServer
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

	CRASH_ASSERT(service->Start(), "SERVICE_START_ERROR");

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

	const std::chrono::seconds CLIENT_TIMEOUT_DURATION(90); // 90초 동안 아무 응답 없으면 타임아웃
	static std::chrono::steady_clock::time_point lastTimeoutCheckTime = std::chrono::steady_clock::now();
	const std::chrono::seconds TIMEOUT_CHECK_INTERVAL(10); // 10초마다 타임아웃 체크 실행

	while (true)
	{
		// 전체 룸 업데이트
		for (int32 i = 1; i <= 5; ++i)
		{
			//RoomManager::Instance().Find(i)->Update(); // JobQueue로 대체됨
			RoomManager::Instance().Find(i)->PushJob(&GameRoom::Update);
		}

		this_thread::sleep_for(50ms);

		for (int32 i = 1; i <= 5; ++i)
		{
			RoomManager::Instance().Find(i)->FlushJob(); // TEST
		}

		// 하트비트
		// 일정 간격으로 타임아웃 클라이언트 체크
		auto currentTime = std::chrono::steady_clock::now();
		if (currentTime - lastTimeoutCheckTime > TIMEOUT_CHECK_INTERVAL)
		{
			// cout << "[Server] Checking for timed out clients..." << endl;
			// GSessionManager는 모든 ClientSession을 관리하는 매니저
			vector<shared_ptr<ClientSession>> sessions = GSessionManager.GetSessions(); // 모든 세션 가져오기
			for (shared_ptr<ClientSession>& session : sessions)
			{
				if (session && session->IsConnected()) // IsConnected()는 세션 활성 상태 확인
				{
					if (session->IsConnectionTimedOut(CLIENT_TIMEOUT_DURATION))
					{
						cout << "[Server] Session ID " << session->SessionId
							<< " timed out. Disconnecting. Last packet received "
							<< chrono::duration_cast<chrono::seconds>(currentTime - session->GetLastPacketReceiveTimeAtomic()).count() // GetLastPacketReceiveTimeAtomic() 추가 필요
							<< " seconds ago." << endl;
						session->Disconnect(L"Heartbeat Timeout");
						// GSessionManager.Remove(session); // Disconnect 내부나 이후에 호출되어야 함
					}
				}
			}
			lastTimeoutCheckTime = currentTime;
		}
	}

#pragma endregion
}