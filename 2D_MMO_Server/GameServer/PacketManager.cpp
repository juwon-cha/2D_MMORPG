
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
    _handler[PacketType::PacketType_C_ENTER_GAME] = PacketHandler::C_ENTER_GAMEHandler;
		_handler[PacketType::PacketType_C_MOVE] = PacketHandler::C_MOVEHandler;
		_handler[PacketType::PacketType_C_CHAT] = PacketHandler::C_CHATHandler;
		_handler[PacketType::PacketType_C_SKILL] = PacketHandler::C_SKILLHandler;
		_handler[PacketType::PacketType_C_EQUIP_ITEM] = PacketHandler::C_EQUIP_ITEMHandler;
		_handler[PacketType::PacketType_C_CHANGE_MAP] = PacketHandler::C_CHANGE_MAPHandler;
		_handler[PacketType::PacketType_C_SIGNUP] = PacketHandler::C_SIGNUPHandler;
		_handler[PacketType::PacketType_D_SIGNUP] = PacketHandler::D_SIGNUPHandler;
		_handler[PacketType::PacketType_C_SIGNIN] = PacketHandler::C_SIGNINHandler;
		_handler[PacketType::PacketType_D_SIGNIN] = PacketHandler::D_SIGNINHandler;
		_handler[PacketType::PacketType_C_HEARTBEAT] = PacketHandler::C_HEARTBEATHandler;
		
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
