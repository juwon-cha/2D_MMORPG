#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// _Header_ciso646 warning
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

#define COMMON_JSON_PATH "../../Common/Data/"

#include "../Libraries/flatbuffers/flatbuffers.h"
#include "../Libraries/nlohmann/json.hpp"

#include "Protocol_generated.h"
//#include <nlohmann/json.hpp>

using namespace flatbuffers;
using namespace nlohmann;

#include "CorePch.h"
#include "Manager.h"
