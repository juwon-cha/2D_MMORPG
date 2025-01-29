#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(int32 bufferSize)
	: _writeSize(0)
{
	_buffer.resize(bufferSize);
}

SendBuffer::~SendBuffer()
{
}

BYTE* SendBuffer::GetBufferData()
{
	return _buffer.data();
}

int32 SendBuffer::GetWriteSize()
{
	return _writeSize;
}

int32 SendBuffer::GetCapacity()
{
	return _buffer.size();
}

void SendBuffer::CopyBuffer(void* data, int32 len)
{
	ASSERT(GetCapacity() >= len, "SendBuffer Overflow");

	::memcpy(_buffer.data(), data, len);
	_writeSize = len;
}
