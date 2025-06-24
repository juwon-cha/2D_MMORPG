#include "pch.h"
#include "PacketHandler.h"
#include "ClientSession.h"
#include "PacketManager.h"

void PacketHandler::C_HEARTBEATHandler(PacketSession* session, ByteRef buffer)
{
    ClientSession* clientSession = static_cast<ClientSession*>(session);
    // clientSession->UpdateLastPacketReceiveTime(); // �̹� ClientSession::OnRecvPacket���� ȣ���

    cout << "[PacketHandler] Received C_HEARTBEAT from Session ID " << clientSession->SessionId << ". Sending S_HEARTBEAT." << endl;

    flatbuffers::FlatBufferBuilder builder;
    // S_HEARTBEAT�� ������ ���ٸ� Start/End�� ȣ��
    //SC_HEARTBEAT::StartSC_HEARTBEAT(builder);
    //auto sHeartbeatOffset = SC_HEARTBEAT::EndSC_HEARTBEAT(builder);

    auto sHeartbeatOffset = CreateSC_HEARTBEAT(builder);

    // �� �κ��� ������Ʈ�� PacketManager ������ ���� �޶���
    auto finalPacket = PacketManager::Instance().CreatePacket(sHeartbeatOffset, builder, PacketType_SC_HEARTBEAT);
    clientSession->Send(finalPacket);
}