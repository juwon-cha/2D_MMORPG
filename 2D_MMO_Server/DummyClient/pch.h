#pragma once

// add headers that you want to pre-compile here
#define WIN32_LEAN_AND_MEAN
// flatbuffers std::min,std::max error
#define NOMINMAX

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore.lib")
#endif

#include "CorePch.h"
