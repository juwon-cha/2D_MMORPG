#include "pch.h"
#include "ThreadPool.h"
#include "Service.h"
#include "Session.h"

#include "../Libraries/flatbuffers/flatbuffers.h"

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
		NetAddress(L"127.0.0.1", 8002),
		std::make_shared<IocpCore>(),
		[]() { return std::make_shared<ServerSession>(); },
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

	while (true){ }
}