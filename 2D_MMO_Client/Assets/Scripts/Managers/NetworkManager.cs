using ServerCore;
using System;
using System.Collections.Generic;
using System.Net;
using UnityEngine;

public class NetworkManager : IManager
{
    Connector _connector = new Connector();
    ServerSession _session = new ServerSession();
    Queue<ArraySegment<byte>> _packetQueue = new Queue<ArraySegment<byte>>();
    object _lock = new object();

    public void Init()
    {
        IPEndPoint endPoint = new IPEndPoint(IPAddress.Loopback, 8002);
        _connector.Init(endPoint, () => { return _session; });
    }

    public void Clear()
    {
    }

    public void Push(ArraySegment<byte> packet)
    {
        //lock (_lock) 
        {
            _packetQueue.Enqueue(packet);
        }
    }

    public void Update()
    {
        //lock (_lock)
        {
            while (_packetQueue.Count > 0) 
                Manager.Packet.OnRecvPacket(_session, _packetQueue.Dequeue());
        }
    }

    public void Send(byte[] buffer)
    {
        _session.Send(buffer);
    }

    public void Disconnect()
    {
        _session.Disconnect();
    }
}