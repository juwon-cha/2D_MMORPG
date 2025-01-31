#pragma once

namespace Utils
{
	class Bitconverter
	{
	public:
		static bool TryWriteBytes(BYTE* destination, uint8 value, uint32 startIndex);
		static bool TryWriteBytes(BYTE* destination, uint16 value, uint32 startIndex);
		static bool TryWriteBytes(BYTE* destination, uint32 value, uint32 startIndex);
		static bool TryWriteBytes(BYTE* destination, uint64 value, uint32 startIndex);
		static bool TryWriteBytes(BYTE* destination, int8 value, uint32 startIndex);
		static bool TryWriteBytes(BYTE* destination, int16 value, uint32 startIndex);
		static bool TryWriteBytes(BYTE* destination, int32 value, uint32 startIndex);
		static bool TryWriteBytes(BYTE* destination, int64 value, uint32 startIndex);
		static uint8 ToUInt8(BYTE* arr, int32 startIndex = 0);
		static uint16 ToUInt16(BYTE* arr, int32 startIndex = 0);
		static uint32 ToUInt32(BYTE* arr, int32 startIndex = 0);
		static uint64 ToUInt64(BYTE* arr, int32 startIndex = 0);
		static int8 ToInt8(BYTE* arr, int32 startIndex = 0);
		static int16 ToInt16(BYTE* arr, int32 startIndex = 0);
		static int32 ToInt32(BYTE* arr, int32 startIndex = 0);
		static int64 ToInt64(BYTE* arr, int32 startIndex = 0);
	};
}