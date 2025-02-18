
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
        _handler.Add((ushort)PacketType.SC_ENTER_GAME, PacketHandler.SC_ENTER_GAME_Handler);
		_handler.Add((ushort)PacketType.SC_LEAVE_GAME, PacketHandler.SC_LEAVE_GAME_Handler);
		_handler.Add((ushort)PacketType.SC_SPAWN, PacketHandler.SC_SPAWN_Handler);
		_handler.Add((ushort)PacketType.SC_DESPAWN, PacketHandler.SC_DESPAWN_Handler);
		_handler.Add((ushort)PacketType.SC_MOVE, PacketHandler.SC_MOVE_Handler);
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
