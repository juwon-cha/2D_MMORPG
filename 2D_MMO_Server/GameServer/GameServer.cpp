#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "ThreadPool.h"
#include "PacketManager.h"

PacketManager& Packet = PacketManager::Instance();

int main()
{
	shared_ptr<ServerService> service = std::make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 8888),
		std::make_shared<IocpCore>(),
		std::make_shared<ClientSession>, // TODO : SessionManager µî
		1);

	ASSERT(service->Start(), "SERVICE_START_ERROR");

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->EnqueueJob([service]() {
			while (true)
			{
				service->GetIocpCore()->Dispatch();
			}
			});
	}

	flatbuffers::FlatBufferBuilder builder;
	auto data = CreateSD_Test(builder, 24);
	auto pkt = Packet.CreatePacket(data, builder, PacketType_SD_Test);
	uint8* flatbuf = builder.GetBufferPointer();

	while (true)
	{
		GSessionManager.Broadcast(pkt);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}