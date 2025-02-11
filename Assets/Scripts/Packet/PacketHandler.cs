using Google.FlatBuffers;
using ServerCore;
using System;
using UnityEngine;

public class PacketHandler
{
    public static void SC_TestHandler(PacketSession session, ByteBuffer buffer)
    {
        var pkt = SC_Test.GetRootAsSC_Test(buffer);
        Debug.Log($"test : {pkt.TestSc}");
    }
}
