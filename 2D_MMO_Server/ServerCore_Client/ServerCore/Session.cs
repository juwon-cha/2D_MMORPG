using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace ServerCore
{
    public abstract class PacketSession : Session
    {
        static readonly ushort HeaderSize = 2;
        public sealed override int OnRecv(ArraySegment<byte> buffer)
        {
            int processLen = 0;

            while (true)
            {
                if (buffer.Count < HeaderSize)
                    break;
                ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
                if (size < 0 || size > buffer.Count)
                    break;
                OnRecvPacket(new ArraySegment<byte>(buffer.Array, buffer.Offset, size));
                processLen += size;

                buffer = new ArraySegment<byte>(buffer.Array, buffer.Offset + size, buffer.Count - size);
            }
            return processLen;
        }
        public abstract void OnRecvPacket(ArraySegment<byte> data);
    }
    public abstract class Session
    {
        Socket _socket;
        int _disconnect = 0;
        object _lock = new object();
        SocketAsyncEventArgs _sendArgs = new SocketAsyncEventArgs();
        SocketAsyncEventArgs _recvArgs = new SocketAsyncEventArgs();
        Queue<byte[]> _sendQueue = new Queue<byte[]>();
        List<ArraySegment<byte>> _pendingList = new List<ArraySegment<byte>>();
        RecvBuffer _recvBuffer = new RecvBuffer(65535);
        public abstract void OnSend(int numOfBytes);
        public abstract int OnRecv(ArraySegment<byte> bytes);
        public abstract void OnConnect(EndPoint endPoint);
        public abstract void OnDisconnect(EndPoint endPoint);

        public void Start(Socket socket)
        {
            _socket = socket;

            _recvArgs.Completed += new EventHandler<SocketAsyncEventArgs>(OnRecvCompleted);
            _sendArgs.Completed += new EventHandler<SocketAsyncEventArgs>(OnSendCompleted);
            RegisterRecv();
        }

        public void Send(ArraySegment<byte> data)
        {
            Send(data.Array);
        }
        public void Send(byte[] data)
        {
            lock (_lock)
            {
                _sendQueue.Enqueue(data);
                if (_pendingList.Count == 0) 
                    RegisterSend();
            }
        }
        void RegisterSend()
        {
            while (_sendQueue.Count > 0)
            {
                var data = _sendQueue.Dequeue();
                _pendingList.Add(new ArraySegment<byte>(data, 0, data.Length));
            }
            _sendArgs.BufferList = _pendingList;
            bool pending = _socket.SendAsync(_sendArgs);
            if (pending == false)
                OnSendCompleted(null, _sendArgs);
        }
        void OnSendCompleted(object sender, SocketAsyncEventArgs args)
        {
            if (args.SocketError == SocketError.Success && args.BytesTransferred > 0)
            {
                _pendingList.Clear();
                OnSend(args.BytesTransferred);
                if (_sendQueue.Count > 0)
                    RegisterSend();
            }
            else
            {
                Console.WriteLine($"OnSendCompleted Failed");
            }
        }
        void RegisterRecv()
        {
            var segment = _recvBuffer.WriteSegment;
            _recvArgs.SetBuffer(segment.Array, segment.Offset, segment.Count);

            bool pending = _socket.ReceiveAsync(_recvArgs);
            if (pending == false)
                OnRecvCompleted(null, _recvArgs);
        }
        void OnRecvCompleted(object sender, SocketAsyncEventArgs args)
        {
            if (args.SocketError == SocketError.Success && args.BytesTransferred > 0)
            {
                if (_recvBuffer.OnWrite((ushort)args.BytesTransferred) == false)
                {
                    Disconnect();
                    return;
                }
                int processLen = OnRecv(_recvBuffer.ReadSegment);
                if (processLen > _recvBuffer.ReadSize)
                {
                    Disconnect();
                    return;
                }
                if (_recvBuffer.OnRead((ushort)processLen) == false)
                {
                    Disconnect();
                    return;
                }
                RegisterRecv();
            }
            else
            {
                Console.WriteLine($"OnRecvCompleted Failed");
                Disconnect();
            }
        }
        public void Disconnect()
        {
            if (Interlocked.Exchange(ref _disconnect, 1) == 1)
                return;
            OnDisconnect(_socket.RemoteEndPoint);
            _socket.Shutdown(SocketShutdown.Both);
            _socket.Close();
        }
    }
}
