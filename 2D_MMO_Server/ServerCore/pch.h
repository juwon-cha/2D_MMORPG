#pragma once

#define WIN32_LEAN_AND_MEAN

#ifdef _DLL_EXPORT
#define COREDLL __declspec (dllexport)
#else if
#define COREDLL __declspec (dllimport)
#endif

#include "CorePch.h"
