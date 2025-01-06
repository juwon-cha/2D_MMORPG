#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"

int main()
{
	shared_ptr<ServerService> service = std::make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 8888),
		std::make_shared<IocpCore>(),
		std::make_shared<ClientSession>, // TODO : SessionManager µî
		10);

	ASSERT(service->Start(), "SERVICE_START_ERROR");

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	char sendData[11] = "HelloWorld";

	while (true)
	{
		BYTE buffer[4096];

		((PacketHeader*)buffer)->size = (sizeof(sendData) + sizeof(PacketHeader));
		((PacketHeader*)buffer)->id = 1; // 1 : Hello Msg
		::memcpy(&buffer[4], sendData, sizeof(sendData));

		GSessionManager.Broadcast(buffer, ((PacketHeader*)buffer)->size);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}