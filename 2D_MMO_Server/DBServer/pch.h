#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define COMMON_JSON_PATH "../Data/"


#include "Protocol_generated.h"
#include <nlohmann/json.hpp>

using namespace flatbuffers;
using json = nlohmann::json;
#include "CorePch.h"
#include "Manager.h"
