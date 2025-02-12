#ifndef FORMAT_H
#define FORMAT_H

#include "pch.h"
#include <initializer_list>

namespace Utils {
	COREDLL string format(const string& input, const initializer_list<string>& args);
	COREDLL wstring wformat(const wstring& input, const initializer_list<wstring>& args);
	COREDLL wstring wformat(const string& input, const initializer_list<string>& args);
	COREDLL wstring wformat(const string& input, const initializer_list<uint64>& args);
	COREDLL wstring wformat(const string& input, const initializer_list<uint32>& args);
}
#endif
