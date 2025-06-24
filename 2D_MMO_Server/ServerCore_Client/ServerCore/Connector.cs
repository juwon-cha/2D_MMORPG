using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace ServerCore
{
    public class Connector
    {
        Func<Session> _sessionFactory;
        public void Init(IPEndPoint endPoint, Func<Session> sessionFactory)
        {
            Socket socket = new Socket(endPoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            // Keep-Alive 옵션 설정
            socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.KeepAlive, true);

            _sessionFactory = sessionFactory;

            SocketAsyncEventArgs args = new SocketAsyncEventArgs();
            args.UserToken = socket;
            args.RemoteEndPoint = endPoint;
            args.Completed += new EventHandler<SocketAsyncEventArgs>(OnConnectCompleted);
            RegisterConnect(args);
        }

        void RegisterConnect(SocketAsyncEventArgs args)
        {
            Socket socket = args.UserToken as Socket;
            bool pending = socket.ConnectAsync(args);

            if (pending == false)
                OnConnectCompleted(null, args);
        }

        void OnConnectCompleted(object sender, SocketAsyncEventArgs args)
        {
            if (args.SocketError == SocketError.Success)
            {
                Session session = _sessionFactory.Invoke();
                session.Start(args.ConnectSocket);
                session.OnConnect(args.ConnectSocket.RemoteEndPoint);
            }
            else
            {
                Console.WriteLine("connected failed");
                RegisterConnect(args);
            }
        }
    }
}
