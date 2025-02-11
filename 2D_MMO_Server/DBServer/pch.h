#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define COMMON_JSON_PATH "../Data/"


#include "Protocol_generated.h"
#include <nlohmann/json.hpp>

using namespace flatbuffers;
using json = nlohmann::json;
#include "CorePch.h"
#include "Manager.h"
