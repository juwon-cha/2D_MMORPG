using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerCore
{
    public class RecvBuffer
    {
        byte[] _buffer;
        ushort _readPos = 0;
        ushort _writePos = 0;
        public ushort ReadSize { get { return (ushort)(_writePos - _readPos); } }
        public ushort WriteSize { get { return (ushort)(_buffer.Length - _writePos); } }
        public ArraySegment<byte> ReadSegment { get { return new ArraySegment<byte>(_buffer, _readPos, ReadSize); } }
        public ArraySegment<byte> WriteSegment { get { return new ArraySegment<byte>(_buffer, _writePos, _buffer.Length - _writePos); } }
        public RecvBuffer(ushort size)
        {
            _buffer = new byte[size];
        }
        public bool OnRead(ushort numOfByte)
        {
            if (numOfByte > ReadSize)
                return false;
            _readPos += numOfByte;
            return true;
        }
        public bool OnWrite(ushort numOfByte)
        {
            if (numOfByte > WriteSize)
                return false;
            _writePos += numOfByte;
            return true;
        }
        public void Clear()
        {
            ushort readSize = ReadSize;
            if (_readPos == _writePos)
            {
                _readPos = _writePos = 0;
            }
            else
            {
                Array.Copy(_buffer, _readPos, _buffer, 0, readSize);
                _readPos = 0;
                _writePos = readSize;
            }
        }
    }
}

//using System;
//// using System.Collections.Generic; // 현재 코드에서는 미사용
//// using System.Linq;              // 현재 코드에서는 미사용
//// using System.Text;              // 현재 코드에서는 미사용
//// using System.Threading.Tasks;   // 현재 코드에서는 미사용

//namespace ServerCore
//{
//    public class RecvBuffer
//    {
//        ArraySegment<byte> _buffer; // 실제 데이터는 이 ArraySegment가 가리키는 배열에 저장됨
//        int _readPos;
//        int _writePos;

//        public RecvBuffer(int bufferSize)
//        {
//            // bufferSize 크기의 byte 배열을 새로 생성하고,
//            // _buffer는 이 전체 배열을 가리키는 ArraySegment로 초기화됩니다.
//            _buffer = new ArraySegment<byte>(new byte[bufferSize], 0, bufferSize);
//        }

//        public int DataSize { get { return _writePos - _readPos; } }
//        public int FreeSize { get { return _buffer.Count - _writePos; } } // 전체 버퍼 크기에서 쓰기 위치를 뺀 만큼이 남은 공간

//        public ArraySegment<byte> ReadSegment
//        {
//            // 읽을 수 있는 데이터 영역을 나타내는 새로운 ArraySegment를 반환
//            // _buffer.Array: 원본 byte[] 배열
//            // _buffer.Offset + _readPos: 실제 데이터 시작 위치
//            // DataSize: 읽을 수 있는 데이터의 총 길이
//            get { return new ArraySegment<byte>(_buffer.Array, _buffer.Offset + _readPos, DataSize); }
//        }

//        public ArraySegment<byte> WriteSegment
//        {
//            // 데이터를 쓸 수 있는 빈 공간 영역을 나타내는 새로운 ArraySegment를 반환
//            // _buffer.Array: 원본 byte[] 배열
//            // _buffer.Offset + _writePos: 데이터를 쓸 수 있는 시작 위치
//            // FreeSize: 쓸 수 있는 공간의 총 크기
//            get { return new ArraySegment<byte>(_buffer.Array, _buffer.Offset + _writePos, FreeSize); }
//        }

//        // --- 새로 추가된 함수 ---
//        /// <summary>
//        /// RecvBuffer가 내부적으로 사용하는 기본 byte[] 배열을 반환합니다.
//        /// Session 클래스에서 SocketAsyncEventArgs에 버퍼를 영구적으로 할당할 때 사용됩니다.
//        /// </summary>
//        /// <returns>내부 byte[] 배열</returns>
//        public byte[] GetUnderlyingBuffer()
//        {
//            return _buffer.Array; // _buffer ArraySegment가 참조하는 실제 byte[] 배열 반환
//        }
//        // --- 함수 추가 끝 ---

//        public void Clean()
//        {
//            int dataSize = DataSize;
//            if (dataSize == 0)
//            {
//                // 읽을 데이터가 없으면 포인터를 모두 0으로 초기화
//                _readPos = _writePos = 0;
//            }
//            else
//            {
//                // 아직 처리하지 않은 데이터가 있다면, 버퍼의 시작 위치로 복사(메모리 재정렬)
//                // _buffer.Array: 원본 배열
//                // _buffer.Offset + _readPos: 현재 읽기 시작 위치
//                // _buffer.Offset: 버퍼의 실제 시작 위치(일반적으로 0)
//                // dataSize: 복사할 데이터의 크기
//                Buffer.BlockCopy(_buffer.Array, _buffer.Offset + _readPos, _buffer.Array, _buffer.Offset, dataSize);
//                _readPos = 0; // 읽기 위치는 맨 앞으로
//                _writePos = dataSize; // 쓰기 위치는 복사된 데이터의 끝으로
//            }
//        }

//        // 데이터를 읽었다고 처리하고 _readPos를 이동시킵니다.
//        // 이전 Session.cs 코드에서는 ushort를 사용했으나, 여기서는 int를 받도록 되어있습니다.
//        // Session.cs에서 이 함수를 호출할 때 (ushort) 캐스팅을 제거하거나, 이 함수의 인자를 ushort로 변경해야 합니다.
//        public bool OnRead(int numOfBytes)
//        {
//            if (numOfBytes <= 0 || numOfBytes > DataSize) // 0 이하의 값도 처리하지 않도록 변경
//                return false;

//            _readPos += numOfBytes;
//            return true;
//        }

//        // 데이터를 썼다고 처리하고 _writePos를 이동시킵니다.
//        // 이전 Session.cs 코드에서는 ushort를 사용했으나, 여기서는 int를 받도록 되어있습니다.
//        public bool OnWrite(int numOfBytes)
//        {
//            if (numOfBytes <= 0 || numOfBytes > FreeSize) // 0 이하의 값도 처리하지 않도록 변경
//                return false;

//            _writePos += numOfBytes;
//            return true;
//        }
//    }
//}
