
using Google.FlatBuffers;
using ServerCore;
using System;
using System.Collections.Generic;

public class PacketManager
{
    Dictionary<ushort, Action<PacketSession, ByteBuffer>> _handler = new Dictionary<ushort, Action<PacketSession, ByteBuffer>>();
    public PacketManager()
    {
        Register();
    }
    void Register()
    {
        _handler.Add((ushort)PacketType.SC_ENTER_GAME, PacketHandler.SC_ENTER_GAMEHandler);
		_handler.Add((ushort)PacketType.SC_LEAVE_GAME, PacketHandler.SC_LEAVE_GAMEHandler);
		_handler.Add((ushort)PacketType.SC_SPAWN, PacketHandler.SC_SPAWNHandler);
		_handler.Add((ushort)PacketType.SC_DESPAWN, PacketHandler.SC_DESPAWNHandler);
		_handler.Add((ushort)PacketType.SC_MOVE, PacketHandler.SC_MOVEHandler);
		_handler.Add((ushort)PacketType.SC_CHAT, PacketHandler.SC_CHATHandler);
		_handler.Add((ushort)PacketType.SC_SKILL, PacketHandler.SC_SKILLHandler);
		_handler.Add((ushort)PacketType.SC_CHANGE_HP, PacketHandler.SC_CHANGE_HPHandler);
		_handler.Add((ushort)PacketType.SC_DIE, PacketHandler.SC_DIEHandler);
		_handler.Add((ushort)PacketType.SC_ITEM_LIST, PacketHandler.SC_ITEM_LISTHandler);
		_handler.Add((ushort)PacketType.SC_EQUIP_ITEM, PacketHandler.SC_EQUIP_ITEMHandler);
		_handler.Add((ushort)PacketType.SC_CHANGE_MAP, PacketHandler.SC_CHANGE_MAPHandler);
		_handler.Add((ushort)PacketType.SC_SIGNUP, PacketHandler.SC_SIGNUPHandler);
		_handler.Add((ushort)PacketType.SC_SIGNIN, PacketHandler.SC_SIGNINHandler);
		
    }
    public void OnRecvPacket(PacketSession session, ArraySegment<byte> buffer)
    {
        ushort count = 0;

        ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
        count += sizeof(ushort);
        ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + count);
        count += sizeof(ushort);

        Action<PacketSession, ByteBuffer> action = null;
        if (_handler.TryGetValue(id, out action))
        {
            byte[] data = new byte[size - count];
            Array.Copy(buffer.Array, buffer.Offset + count, data, 0, size - count);
            ByteBuffer bb = new ByteBuffer(data);
            action.Invoke(session, bb);
        }
    }
    ushort PacketHederSize = 4;
    public byte[] CreatePacket<T>(Offset<T> data, FlatBufferBuilder builder, PacketType id) where T : struct
    {
        builder.Finish(data.Value);
        var bytes = builder.SizedByteArray();
        ushort size = (ushort)(bytes.Length + PacketHederSize);
        ArraySegment<byte> segment = new ArraySegment<byte>(new byte[size]);

        bool success = true;
        success &= BitConverter.TryWriteBytes(new Span<byte>(segment.Array, 0, 2), size);
        success &= BitConverter.TryWriteBytes(new Span<byte>(segment.Array, 2, 4), (ushort)id);
        Buffer.BlockCopy(bytes, 0, segment.Array, 4, bytes.Length);

        if (success == false)
            return null;
        return segment.Array;
    }
}
