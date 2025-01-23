#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"

#include "flatbuffers/flatbuffers.h"
#include "PlayerInfo_generated.h"

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

	// char sendData[11] = "HelloWorld";

	flatbuffers::FlatBufferBuilder builder;
	{
		flatbuffers::Offset<flatbuffers::String> name = builder.CreateString("FromServer!");
		int level = 1;
		flatbuffers::Offset<PlayerInfo> player = CreatePlayerInfo(builder, name, level);
		builder.Finish(player);
	}
	uint8* flatbuf = builder.GetBufferPointer();

	while (true)
	{
		BYTE buffer[4096];

		//((PacketHeader*)buffer)->size = (sizeof(sendData) + sizeof(PacketHeader));
		((PacketHeader*)buffer)->size = (builder.GetSize() + sizeof(PacketHeader));
		((PacketHeader*)buffer)->id = 1; // 1 : Hello Msg
		//::memcpy(&buffer[4], sendData, sizeof(sendData));
		::memcpy(&buffer[4], flatbuf, builder.GetSize());

		GSessionManager.Broadcast(buffer, ((PacketHeader*)buffer)->size);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}