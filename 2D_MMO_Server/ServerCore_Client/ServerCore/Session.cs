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
        int _disconnect = 0; // 0: 연결됨, 1: 연결 끊김 처리 시작
        object _lock = new object();
        SocketAsyncEventArgs _sendArgs = new SocketAsyncEventArgs();
        SocketAsyncEventArgs _recvArgs = new SocketAsyncEventArgs();
        Queue<byte[]> _sendQueue = new Queue<byte[]>();
        //Queue<ArraySegment<byte>> _sendQueue = new Queue<ArraySegment<byte>>();
        List<ArraySegment<byte>> _pendingList = new List<ArraySegment<byte>>();
        RecvBuffer _recvBuffer = new RecvBuffer(65535);
        public abstract void OnSend(int numOfBytes);
        public abstract int OnRecv(ArraySegment<byte> bytes);
        public abstract void OnConnect(EndPoint endPoint);
        public abstract void OnDisconnect(EndPoint endPoint);

        public bool IsConnected // 현재 연결 상태를 확인하는 프로퍼티 추가
        {
            get { return _disconnect == 0 && _socket != null && _socket.Connected; }
        }

        // 기존 코드
        public void Start(Socket socket)
        {
            _socket = socket;

            _recvArgs.Completed += new EventHandler<SocketAsyncEventArgs>(OnRecvCompleted);
            _sendArgs.Completed += new EventHandler<SocketAsyncEventArgs>(OnSendCompleted);
            RegisterRecv();
        }

        //public void Start(Socket socket)
        //{
        //    _socket = socket;
        //    _disconnect = 0; // 새로 Start 시 _disconnect 플래그 초기화

        //    _sendArgs = new SocketAsyncEventArgs(); // SAEA 객체 새로 생성 (재사용 시 이전 상태 문제 방지)
        //    _sendArgs.Completed += OnSendCompleted;

        //    _recvArgs = new SocketAsyncEventArgs(); // SAEA 객체 새로 생성
        //    _recvArgs.Completed += OnRecvCompleted;

        //    byte[] mainRecvBufferArray = _recvBuffer.GetUnderlyingBuffer();
        //    if (mainRecvBufferArray != null)
        //    {
        //        _recvArgs.SetBuffer(mainRecvBufferArray, 0, mainRecvBufferArray.Length);
        //    }
        //    else
        //    {
        //        Logger.LogError("[Session] RecvBuffer's underlying buffer is null. Critical error.");
        //        ForceDisconnect(true, "RecvBuffer init failed"); // 내부 강제 종료 함수 호출
        //        return;
        //    }
        //    RegisterRecv();
        //}

        // 기존 코드
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

        //public void Send(ArraySegment<byte> sendBuff)
        //{
        //    if (!IsConnected) // _disconnect 플래그 대신 IsConnected 사용
        //    {
        //        // Logger.LogWarning("[Session] Send called on not connected session. Ignoring.");
        //        return;
        //    }

        //    lock (_lock)
        //    {
        //        _sendQueue.Enqueue(sendBuff); // ArraySegment 직접 Enqueue (Send(byte[])는 삭제 또는 내부에서 변환)
        //        if (_pendingList.Count == 0) // 현재 진행 중인 SendAsync 작업이 없을 때만 RegisterSend 호출
        //            RegisterSend();
        //    }
        //}

        // 기존 코드
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

        //void RegisterSend()
        //{
        //    if (!IsConnected)
        //    {
        //        // Logger.LogWarning("[Session] RegisterSend called on not connected session.");
        //        return;
        //    }

        //    _pendingList.Clear();
        //    lock (_lock)
        //    {
        //        while (_sendQueue.Count > 0)
        //        {
        //            _pendingList.Add(_sendQueue.Dequeue());
        //        }
        //    }

        //    if (_pendingList.Count == 0)
        //        return;

        //    _sendArgs.BufferList = _pendingList;

        //    try
        //    {
        //        if (!IsConnected)
        //        { // SendAsync 직전 최종 확인
        //            // Logger.LogWarning("[Session] RegisterSend: Socket not connected before SendAsync.");
        //            return; // 이미 연결이 끊겼으면 SendAsync 호출 안 함
        //        }
        //        bool pending = _socket.SendAsync(_sendArgs);
        //        if (pending == false)
        //            OnSendCompleted(null, _sendArgs);
        //    }
        //    // (이하 ObjectDisposedException, SocketException, 일반 Exception 처리는 이전 답변과 동일하게 유지)
        //    catch (ObjectDisposedException ode) { Logger.LogError($"[Session] RegisterSend ODE: {ode.Message}"); ForceDisconnect(true, "RegisterSend ODE"); }
        //    catch (SocketException se) { Logger.LogError($"[Session] RegisterSend SE: {se.SocketErrorCode} - {se.Message}"); ForceDisconnect(true, "RegisterSend SE"); }
        //    catch (Exception e) { Logger.LogError($"[Session] RegisterSend Exception: {e.GetType().Name} - {e.Message}\n{e.StackTrace}"); ForceDisconnect(true, "RegisterSend Exception"); }
        //}

        // 기존 코드
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

        //void OnSendCompleted(object sender, SocketAsyncEventArgs args)
        //{
        //    // 이미 연결 끊김 처리가 시작되었고, 에러로 완료된 경우 추가 처리 방지
        //    if (_disconnect == 1 && args.SocketError != SocketError.Success) return;

        //    try
        //    {
        //        if (args.SocketError == SocketError.Success && args.BytesTransferred > 0)
        //        {
        //            OnSend(args.BytesTransferred);
        //            args.BufferList = null;
        //            _pendingList.Clear();

        //            bool startNextSend = false;
        //            lock (_lock) { if (_sendQueue.Count > 0) startNextSend = true; }
        //            if (startNextSend) RegisterSend();
        //        }
        //        else
        //        {
        //            Logger.LogError($"[Session] OnSendCompleted Failed. Error: {args.SocketError}, Bytes: {args.BytesTransferred}");
        //            ForceDisconnect(false, $"SendFail: {args.SocketError}"); // 사용자가 Disconnect를 호출하지 않았으므로 isGraceful=false
        //        }
        //    }
        //    // (이하 Exception 처리는 이전 답변과 동일하게 유지)
        //    catch (ObjectDisposedException ode) { Logger.LogError($"[Session] OnSendCompleted ODE: {ode.Message}"); /* 이미 Disconnect 처리 중일 수 있음 */ }
        //    catch (Exception e) { Logger.LogError($"[Session] OnSendCompleted Exception: {e.GetType().Name} - {e.Message}\n{e.StackTrace}"); ForceDisconnect(false, "SendCallback Exception"); }
        //}

        // 기존 코드
        void RegisterRecv()
        {
            var segment = _recvBuffer.WriteSegment;
            _recvArgs.SetBuffer(segment.Array, segment.Offset, segment.Count);

            bool pending = _socket.ReceiveAsync(_recvArgs);
            if (pending == false)
                OnRecvCompleted(null, _recvArgs);
        }

        //void RegisterRecv()
        //{
        //    if (!IsConnected) return;

        //    _recvBuffer.Clean();
        //    ArraySegment<byte> segment = _recvBuffer.WriteSegment;
        //    // SetBuffer는 이미 Start에서 전체 배열로 설정됨. 여기서는 Offset과 Count만 재설정.
        //    _recvArgs.SetBuffer(_recvArgs.Buffer, segment.Offset, segment.Count);

        //    try
        //    {
        //        if (!IsConnected) return;
        //        bool pending = _socket.ReceiveAsync(_recvArgs);
        //        if (pending == false)
        //            OnRecvCompleted(null, _recvArgs);
        //    }
        //    // (이하 Exception 처리는 이전 답변과 동일하게 유지)
        //    catch (ObjectDisposedException ode) { Logger.LogError($"[Session] RegisterRecv ODE: {ode.Message}"); ForceDisconnect(true, "RegisterRecv ODE"); }
        //    catch (SocketException se) { Logger.LogError($"[Session] RegisterRecv SE: {se.SocketErrorCode} - {se.Message}"); ForceDisconnect(true, "RegisterRecv SE"); }
        //    catch (Exception e) { Logger.LogError($"[Session] RegisterRecv Exception: {e.GetType().Name} - {e.Message}\n{e.StackTrace}"); ForceDisconnect(true, "RegisterRecv Exception"); }
        //}

        // 기존 코드
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
        //void OnRecvCompleted(object sender, SocketAsyncEventArgs args)
        //{
        //    if (_disconnect == 1 && args.SocketError != SocketError.Success) return;

        //    try
        //    {
        //        if (args.SocketError == SocketError.Success && args.BytesTransferred > 0)
        //        {
        //            if (!_recvBuffer.OnWrite((ushort)args.BytesTransferred)) { Logger.LogError("[Session] OnRecv OnWrite fail"); ForceDisconnect(false, "Recv OnWrite fail"); return; }
        //            int processLen = OnRecv(_recvBuffer.ReadSegment);
        //            if (processLen < 0 || _recvBuffer.DataSize < processLen) { Logger.LogError($"[Session] OnRecv Invalid processLen: {processLen}"); ForceDisconnect(false, "Recv Invalid processLen"); return; }
        //            if (!_recvBuffer.OnRead((ushort)processLen)) { Logger.LogError("[Session] OnRecv OnRead fail"); ForceDisconnect(false, "Recv OnRead fail"); return; }
        //            RegisterRecv();
        //        }
        //        else
        //        {
        //            Logger.LogError($"[Session] OnRecvCompleted indicates connection issue. Error: {args.SocketError}, Bytes: {args.BytesTransferred}");
        //            ForceDisconnect(args.SocketError == SocketError.Success, $"RecvFail: {args.SocketError}"); // BytesTransferred == 0 이면 정상 종료일 수 있음
        //        }
        //    }
        //    // (이하 Exception 처리는 이전 답변과 동일하게 유지)
        //    catch (ObjectDisposedException ode) { Logger.LogError($"[Session] OnRecvCompleted ODE: {ode.Message}"); /* 이미 Disconnect 처리 중일 수 있음 */ }
        //    catch (Exception e) { Logger.LogError($"[Session] OnRecvCompleted Exception: {e.GetType().Name} - {e.Message}\n{e.StackTrace}"); ForceDisconnect(false, "RecvCallback Exception"); }
        //}

        // 기존 코드
        public void Disconnect()
        {
            if (Interlocked.Exchange(ref _disconnect, 1) == 1)
                return;

            // Disconnect가 어디서 호출되었는지 추적하기 위해 스택 트레이스를 로그로 남깁니다.
            Logger.LogError($"[Session] Disconnect() Called! Closing socket. Endpoint: {_socket?.RemoteEndPoint}\n--- StackTrace ---\n{Environment.StackTrace}");

            OnDisconnect(_socket.RemoteEndPoint);
            _socket.Shutdown(SocketShutdown.Both);
            _socket.Close();
        }

        // Disconnect는 외부(예: NetworkManager, 게임 로직)에서 호출될 수 있는 공개 메소드
        //public void Disconnect(string reason = "User Initiated")
        //{
        //    ForceDisconnect(true, reason); // 사용자가 호출한 Disconnect는 정상 종료로 간주 (isGraceful=true)
        //}

        //// 실제 연결 종료 처리를 담당하는 내부 함수
        //// isGraceful: 정상적인 종료 요청인지 (예: 사용자가 종료 버튼 누름, 서버가 FIN 보냄)
        ////             아니면 오류로 인한 강제 종료인지 구분
        //private void ForceDisconnect(bool isGraceful, string reason)
        //{
        //    if (Interlocked.Exchange(ref _disconnect, 1) == 1)
        //        return;

        //    // 최초 Disconnect 호출 스택 트레이스 기록
        //    Logger.LogError($"[Session] ForceDisconnect (Graceful: {isGraceful}) Called! Reason: {reason}. Endpoint: {_socket?.RemoteEndPoint}\n--- StackTrace ---\n{Environment.StackTrace}");

        //    try { OnDisconnect(_socket?.RemoteEndPoint); } // 사용자 정의 콜백
        //    catch (Exception e) { Logger.LogError($"[Session] Exception in user OnDisconnect: {e.Message}\n{e.StackTrace}"); }

        //    try
        //    {
        //        if (_socket != null)
        //        {
        //            _socket.Shutdown(SocketShutdown.Both);
        //            _socket.Close();
        //        }
        //    }
        //    catch (ObjectDisposedException) { /* 이미 다른 곳에서 닫혔을 수 있음, 무시 */ }
        //    catch (Exception e) { Logger.LogError($"[Session] Exception during socket shutdown: {e.GetType().Name} - {e.Message}"); }
        //    finally
        //    {
        //        _socket = null;
        //        // SAEA 정리 (재사용하지 않을 경우)
        //        if (_sendArgs != null) { _sendArgs.Completed -= OnSendCompleted; _sendArgs.Dispose(); _sendArgs = null; }
        //        if (_recvArgs != null) { _recvArgs.Completed -= OnRecvCompleted; _recvArgs.Dispose(); _recvArgs = null; }
        //    }
        //}
    }
}
