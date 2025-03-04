#pragma once

class StringConverter
{
public:
	static std::wstring ConvertStringToWString(const char* ansi, const size_t ansi_size);
};
