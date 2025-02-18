using ServerCore;
using System;
using System.Net;
using UnityEngine;

public class ServerSession : PacketSession
{
    public override void OnConnect(EndPoint endPoint)
    {
        Debug.Log("OnConnect");
    }
    public override void OnDisconnect(EndPoint endPoint)
    {
        Debug.Log("OnDisconnect");
    }

    public override void OnRecvPacket(ArraySegment<byte> data)
    {
        Manager.Network.Push(data);
    }
    public override void OnSend(int numOfBytes)
    {
        //Debug.Log("OnSend");
    }
}
