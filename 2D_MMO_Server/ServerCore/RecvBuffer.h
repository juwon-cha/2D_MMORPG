#pragma once

/*
[r][][w][][][][][]
writePos: ���ۿ� �����͸� ���� ������ ũ�� ��ŭ writePos �̵�
readPos: �о���� ������ ��ġ, writePos�� readPos ������ �����Ͱ� ��ȿ ������

[][][][][rw][][][]
writePos�� readPos�� ������ ��� ������ ó�� -> writePos�� readPos ��� 0�� ��ġ�� �̵�

[][][][][][r][][w]
writePos�� ������ ������ �Դµ� ó���ؾ��� �����Ͱ� �ִٸ� �����͸� ������ 0�� ��ġ�� ����
*/

class RecvBuffer
{
	// ���� ũ�⸦ �÷� ������ ���� ��� ����
	// -> �ִ��� writePos�� readPos�� ��ġ�� ����� ����
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

