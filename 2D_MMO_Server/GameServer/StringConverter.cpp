#include "pch.h"
#include "StringConverter.h"

std::wstring StringConverter::ConvertStringToWString(const char* ansi, const size_t ansiSize)
{
	wstring wstr = L"";

	if ((nullptr == ansi) || (0 == ansiSize))
	{
		return L"";
	}

	// �����ڵ�� ��ȯ�ϱ� �ռ� ���̸� �˾ƾ� ��
	int32 length = ::MultiByteToWideChar(CP_ACP, 0, ansi, static_cast<int32>(ansiSize), NULL, NULL);
	if (0 >= length)
	{
		return L"";
	}
	else
	{
		wstr.resize(length);

		//�����ڵ�� ��ȯ
		if (0 >= ::MultiByteToWideChar(CP_ACP, 0, ansi, static_cast<int32>(ansiSize), &wstr[0], static_cast<int32>(wstr.size())))
		{
			return L"";
		}
	}

	return wstr;
}
