#include "pch.h"
#include "StringConverter.h"

std::wstring StringConverter::ConvertStringToWString(const char* ansi, const size_t ansiSize)
{
	wstring wstr = L"";

	if ((nullptr == ansi) || (0 == ansiSize))
	{
		return L"";
	}

	// 유니코드로 변환하기 앞서 길이를 알아야 함
	int32 length = ::MultiByteToWideChar(CP_ACP, 0, ansi, static_cast<int32>(ansiSize), NULL, NULL);
	if (0 >= length)
	{
		return L"";
	}
	else
	{
		wstr.resize(length);

		//유니코드로 변환
		if (0 >= ::MultiByteToWideChar(CP_ACP, 0, ansi, static_cast<int32>(ansiSize), &wstr[0], static_cast<int32>(wstr.size())))
		{
			return L"";
		}
	}

	return wstr;
}
