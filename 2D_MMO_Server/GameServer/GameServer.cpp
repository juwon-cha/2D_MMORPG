#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"

#include "../Libraries/flatbuffers/flatbuffers.h"
#include "PlayerInfo_generated.h"

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
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

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
		shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(4096);

		BYTE* buffer = sendBuffer->GetBufferData();
		((PacketHeader*)buffer)->size = (builder.GetSize() + sizeof(PacketHeader));
		((PacketHeader*)buffer)->id = 1; // temp packet ID
		::memcpy(&buffer[4], flatbuf, builder.GetSize());

		sendBuffer->CopyBuffer(buffer, ((PacketHeader*)buffer)->size);

		GSessionManager.Broadcast(sendBuffer);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}