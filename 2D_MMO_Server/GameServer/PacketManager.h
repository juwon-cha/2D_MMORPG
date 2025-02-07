
#pragma once
#include <functional>
#include <unordered_map>
#include "Session.h"
#include "BitConverter.h"
#include "Array.h"
#include "Types.h"

class PacketManager
{
private:
    static PacketManager* _instance;
    unordered_map<int, function<void(PacketSession*, ByteRef&)> > _handler;
    void Register();
    PacketManager();

public:
    static PacketManager& Instance();
    ~PacketManager();
    void OnRecvPacket(PacketSession* session, BYTE* buffer);
    template <typename T>
    ByteRef CreatePacket(Offset<T>& data, FlatBufferBuilder& builder, PacketType id);
};

template<typename T>
inline SendBufferRef PacketManager::CreatePacket(Offset<T>& data, FlatBufferBuilder& builder, PacketType id)
{
    builder.Finish(data);
    auto size = builder.GetSize() + 4;
    BYTE* bb = new BYTE[size];
    Utils::BitConverter::TryWriteBytes(bb, size, 0);
    Utils::BitConverter::TryWriteBytes(bb, id, 2);
    
    auto* pkt = builder.GetBufferPointer();
    Utils::Array::Copy((BYTE*)pkt, 0, bb, 4, size - 4);
    SendBufferRef buf = make_shared<SendBuffer>(size);
    buf->CopyBuffer(bb, size);
    return buf;
}
