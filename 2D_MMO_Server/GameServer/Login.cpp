#include "pch.h"
#include "PacketHandler.h"
#include "ClientSession.h"
#include "Player.h"
#include "GameRoom.h"
#include "PacketManager.h"
#include "ClientSessionManager.h"
#include "RoomManager.h"

void PacketHandler::C_ENTER_GAMEHandler(PacketSession* session, ByteRef buffer) {
	auto c = reinterpret_cast<ClientSession*>(session);
	auto client = GSessionManager.Find(c->SessionId);

	if (client == nullptr) return;
	if (client->DbId == 0)
	{
		client->Disconnect(L"INVALID CLIENT");
		return;
	}
	RoomManager::Instance().Find(1)->EnterGame(client->GetPlayer());
}

void PacketHandler::C_SIGNUPHandler(PacketSession* session, ByteRef buffer) {
	auto pkt = GetRoot<C_SIGNUP>(buffer->operator unsigned char* ());

	auto c = reinterpret_cast<ClientSession*>(session);
	auto client = GSessionManager.Find(c->SessionId);

	if (client == nullptr) return;
	try {
		FlatBufferBuilder builder;
		auto id = pkt->id()->c_str();
		auto password = pkt->password()->c_str();
		if (::strlen(id) == 0 || ::strlen(password) == 0)
		{
			client->Disconnect(L"INVALID CLIENT");
			return;
		}
		auto data = CreateSD_SIGNUPDirect(builder, client->SessionId, id, password);
		auto packet = PacketManager::Instance().CreatePacket(data, builder, PacketType_SD_SIGNUP);
		GSessionManager.DB->Send(packet);
	}
	catch (...)
	{
		FlatBufferBuilder builder;
		auto data = CreateSC_SIGNUP(builder, SignUpError_UNKNOWN);
		auto packet = PacketManager::Instance().CreatePacket(data, builder, PacketType_SD_SIGNUP);
		GSessionManager.DB->Send(packet);
	}
}

void PacketHandler::D_SIGNUPHandler(PacketSession* session, ByteRef buffer) {
	auto pkt = GetRoot<D_SIGNUP>(buffer->operator unsigned char* ());

	auto client = GSessionManager.Find(pkt->session_id());
	if (client == nullptr) return;
	client->DbId = pkt->db_id();
	try {
		FlatBufferBuilder builder;

		auto ok = pkt->ok();
		auto data = CreateSC_SIGNUP(builder, ok);
		auto packet = PacketManager::Instance().CreatePacket(data, builder, PacketType_SC_SIGNUP);
		client->Send(packet);
	}
	catch (...)
	{

	}
}

void PacketHandler::C_SIGNINHandler(PacketSession* session, ByteRef buffer) {
	auto pkt = GetRoot<C_SIGNIN>(buffer->operator unsigned char* ());

	auto c = reinterpret_cast<ClientSession*>(session);
	auto client = GSessionManager.Find(c->SessionId);

	if (client == nullptr) return;
	try {
		FlatBufferBuilder builder;
		auto id = pkt->id()->c_str();
		auto password = pkt->password()->c_str();
		if (::strlen(id) == 0 || ::strlen(password) == 0)
		{
			client->Disconnect(L"INVALID CLIENT");
			return;
		}
		auto data = CreateSD_SIGNINDirect(builder, client->SessionId, id, password);
		auto packet = PacketManager::Instance().CreatePacket(data, builder, PacketType_SD_SIGNIN);
		GSessionManager.DB->Send(packet);
	}
	catch (...)
	{
		FlatBufferBuilder builder;
		auto data = CreateSC_SIGNIN(builder, SignInError_UNKNOWN);
		auto packet = PacketManager::Instance().CreatePacket(data, builder, PacketType_SC_SIGNIN);
		client->Send(packet);
	}
}

void PacketHandler::D_SIGNINHandler(PacketSession* session, ByteRef buffer) {
	auto pkt = GetRoot<D_SIGNIN>(buffer->operator unsigned char* ());

	auto client = GSessionManager.Find(pkt->session_id());
	if (client == nullptr) return;
	client->DbId = pkt->db_id();

	try {
		FlatBufferBuilder builder;
		auto ok = pkt->ok();
		auto data = CreateSC_SIGNIN(builder, ok);
		auto packet = PacketManager::Instance().CreatePacket(data, builder, PacketType_SC_SIGNIN);
		client->Send(packet);
	}
	catch (...)
	{

	}
}