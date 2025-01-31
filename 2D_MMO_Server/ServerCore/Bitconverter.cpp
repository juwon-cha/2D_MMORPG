#include "pch.h"
#include "Bitconverter.h"

template <typename T>
bool WriteBytes(T value, BYTE* bytes, uint32 startIndex) {
	::memcpy(&bytes[startIndex], &value, sizeof(value));
	return true;
}

template <typename T>
T ToInt(BYTE* bytes, uint32 startIndex) {
	T value;
	::memcpy(&value, &bytes[startIndex], sizeof(value));
	return value;
}

namespace Utils
{
	bool Bitconverter::TryWriteBytes(BYTE* destination, uint8 value, uint32 startIndex)
	{
		return WriteBytes(value, destination, startIndex);
	}

	bool Bitconverter::TryWriteBytes(BYTE* destination, uint16 value, uint32 startIndex)
	{
		return WriteBytes(value, destination, startIndex);
	}

	bool Bitconverter::TryWriteBytes(BYTE* destination, uint32 value, uint32 startIndex)
	{
		return WriteBytes(value, destination, startIndex);
	}

	bool Bitconverter::TryWriteBytes(BYTE* destination, uint64 value, uint32 startIndex)
	{
		return WriteBytes(value, destination, startIndex);
	}

	bool Bitconverter::TryWriteBytes(BYTE* destination, int8 value, uint32 startIndex)
	{
		return WriteBytes(value, destination, startIndex);
	}

	bool Bitconverter::TryWriteBytes(BYTE* destination, int16 value, uint32 startIndex)
	{
		return WriteBytes(value, destination, startIndex);
	}

	bool Bitconverter::TryWriteBytes(BYTE* destination, int32 value, uint32 startIndex)
	{
		return WriteBytes(value, destination, startIndex);
	}

	bool Bitconverter::TryWriteBytes(BYTE* destination, int64 value, uint32 startIndex)
	{
		return WriteBytes(value, destination, startIndex);
	}

	uint8 Bitconverter::ToUInt8(BYTE* arr, int32 startIndex)
	{
		return ToInt<uint8>(arr, startIndex);
	}

	uint16 Bitconverter::ToUInt16(BYTE* arr, int32 startIndex)
	{
		return ToInt<uint16>(arr, startIndex);
	}

	uint32 Bitconverter::ToUInt32(BYTE* arr, int32 startIndex)
	{
		return ToInt<uint32>(arr, startIndex);
	}

	uint64 Bitconverter::ToUInt64(BYTE* arr, int32 startIndex)
	{
		return ToInt<uint64>(arr, startIndex);
	}

	int8 Bitconverter::ToInt8(BYTE* arr, int32 startIndex)
	{
		return ToInt<int8>(arr, startIndex);
	}

	int16 Bitconverter::ToInt16(BYTE* arr, int32 startIndex)
	{
		return ToInt<int16>(arr, startIndex);
	}

	int32 Bitconverter::ToInt32(BYTE* arr, int32 startIndex)
	{
		return ToInt<int32>(arr, startIndex);
	}

	int64 Bitconverter::ToInt64(BYTE* arr, int32 startIndex)
	{
		return ToInt<int64>(arr, startIndex);
	}
}
