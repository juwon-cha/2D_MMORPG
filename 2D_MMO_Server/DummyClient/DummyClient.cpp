#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"

#include "flatbuffers/flatbuffers.h"
#include "PlayerInfo_generated.h"

class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		cout << "Connected To Server" << endl;
		//Send((BYTE*)sendBuffer, sizeof(sendBuffer));
	}

	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override
	{
		PacketHeader header = *((PacketHeader*)buffer);
		cout << "Packet ID : " << header.id << " Size : " << header.size << endl;

		cout << "OnRecv Len = " << len << endl;

		this_thread::sleep_for(1s);

		char recvBuffer[4096];
		uint8* flatbuffer = (uint8*)&buffer[4];
		const PlayerInfo* player = GetPlayerInfo(flatbuffer);
		//::memcpy(recvBuffer, &buffer[4], header.size - sizeof(PacketHeader));
		//cout << recvBuffer << endl;
		cout << "Name: " << player->name()->c_str() << " Level: " << player->level() << endl;

		Send((BYTE*)buffer, sizeof(buffer));

		return len;
	}

	virtual void OnSend(int32 len) override
	{
		//cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		cout << "Disconnected" << endl;
	}
};

int main()
{
	this_thread::sleep_for(1s);

	shared_ptr<ClientService> service = std::make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 8888),
		std::make_shared<IocpCore>(),
		std::make_shared<ServerSession>, // TODO : SessionManager µî
		5);

	ASSERT(service->Start(), "SERVICE_START_ERROR");

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}