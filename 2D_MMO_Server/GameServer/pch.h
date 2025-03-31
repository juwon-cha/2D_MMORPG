#pragma once

// add headers that you want to pre-compile here
#define WIN32_LEAN_AND_MEAN
// flatbuffers std::min,std::max error
#define NOMINMAX
// _Header_ciso646 warning
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING


#include "../Libraries/flatbuffers/flatbuffers.h"
#include "../Libraries/nlohmann/json.hpp"
#include "Protocol_generated.h"
using namespace flatbuffers;
using namespace nlohmann;
#include "CorePch.h"
