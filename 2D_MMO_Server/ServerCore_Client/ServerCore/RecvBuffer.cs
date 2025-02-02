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
