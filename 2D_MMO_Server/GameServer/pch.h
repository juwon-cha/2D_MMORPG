#pragma once

// add headers that you want to pre-compile here
#define WIN32_LEAN_AND_MEAN
// flatbuffers std::min,std::max error
#define NOMINMAX

#include "../Libraries/flatbuffers/flatbuffers.h"
#include "Protocol_generated.h"
using namespace flatbuffers;
#include "CorePch.h"
