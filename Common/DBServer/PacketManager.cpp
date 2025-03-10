
#include "pch.h"
#include "PacketManager.h"
#include "PacketHandler.h"
#include "CoreRef.h"

PacketManager* PacketManager::_instance = nullptr;

PacketManager& PacketManager::Instance()
{
	if (_instance == nullptr)
		_instance = new PacketManager();
	return *_instance;
}

PacketManager::PacketManager()
{
	Register();
}

PacketManager::~PacketManager()
{
}

void PacketManager::Register()
{
    
}

void PacketManager::OnRecvPacket(PacketSession* session, BYTE* buffer)
{
	uint16 count = 0;

	uint16 size = Utils::BitConverter::ToUInt16(buffer, count);
	count += sizeof(uint16);
	uint16 id = Utils::BitConverter::ToUInt16(buffer, count);
	count += sizeof(uint16);

	auto func = _handler.find(id);
	if (func != _handler.end())
	{
		size -= count;
		ByteRef buf = make_shared<BaseRef<BYTE>>();

		buf->Reserve(size);
		buf->Copy(buffer + count, size);
		func->second(session, buf);
	}
}
