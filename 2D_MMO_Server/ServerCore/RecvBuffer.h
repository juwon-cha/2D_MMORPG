#pragma once

/*
[r][][w][][][][][]
writePos: 버퍼에 데이터를 쓰면 데이터 크기 만큼 writePos 이동
readPos: 읽어야할 데이터 위치, writePos와 readPos 사이의 데이터가 유효 데이터

[][][][][rw][][][]
writePos와 readPos가 같으면 모든 데이터 처리 -> writePos와 readPos 모두 0번 위치로 이동

[][][][][][r][][w]
writePos가 버퍼의 끝까지 왔는데 처리해야할 데이터가 있다면 데이터를 버퍼의 0번 위치로 복사
*/

class RecvBuffer
{
	// 버퍼 크기를 늘려 데이터 복사 비용 줄임
	// -> 최대한 writePos와 readPos를 겹치게 만들기 위함
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();

	void Clean();
	bool OnRead(int32 numOfBytes);
	bool OnWrite(int32 numOfBytes);

	BYTE* ReadPos() { return &_buffer[_readPos]; }
	BYTE* WritePos() { return &_buffer[_writePos]; }
	int32 DataSize() const { return _writePos - _readPos; }
	int32 FreeSize() const { return _capacity - _writePos; }

private:
	int32 _capacity = 0;
	int32 _bufferSize = 0;
	int32 _readPos = 0;
	int32 _writePos = 0;
	vector<BYTE> _buffer;
};

