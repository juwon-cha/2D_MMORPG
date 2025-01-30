#pragma once

class COREDLL SendBuffer : enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(int32 bufferSize);
	~SendBuffer();

	BYTE* GetBufferData();
	int32 GetWriteSize();
	int32 GetCapacity();

	void CopyBuffer(void* data, int32 len);

private:
	vector<BYTE> _buffer;
	int32 _writeSize; // 실제 버퍼를 사용하는 크기. 버퍼 크기 보다 작을 수 있음
};

