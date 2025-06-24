#include "pch.h"
#include "PacketHandler.h"
#include "ClientSession.h"
#include "PacketManager.h"

void PacketHandler::C_HEARTBEATHandler(PacketSession* session, ByteRef buffer)
{
    ClientSession* clientSession = static_cast<ClientSession*>(session);
    // clientSession->UpdateLastPacketReceiveTime(); // 이미 ClientSession::OnRecvPacket에서 호출됨

    cout << "[PacketHandler] Received C_HEARTBEAT from Session ID " << clientSession->SessionId << ". Sending S_HEARTBEAT." << endl;

    flatbuffers::FlatBufferBuilder builder;
    // S_HEARTBEAT에 내용이 없다면 Start/End만 호출
    //SC_HEARTBEAT::StartSC_HEARTBEAT(builder);
    //auto sHeartbeatOffset = SC_HEARTBEAT::EndSC_HEARTBEAT(builder);

    auto sHeartbeatOffset = CreateSC_HEARTBEAT(builder);

    // 이 부분은 프로젝트의 PacketManager 구조에 따라 달라짐
    auto finalPacket = PacketManager::Instance().CreatePacket(sHeartbeatOffset, builder, PacketType_SC_HEARTBEAT);
    clientSession->Send(finalPacket);
}