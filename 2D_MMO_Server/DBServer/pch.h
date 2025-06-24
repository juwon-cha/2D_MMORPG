#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
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
