#pragma once

#include "Types.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

#include <array>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "CoreRef.h"

// Winsock 헤더는 windows.h 보다 반드시 먼저 와야 한다
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <mstcpip.h>  // tcp_keepalive
#pragma comment(lib, "ws2_32.lib")

#include <cassert>
#include <stdexcept>
#include <windows.h>
#include <iostream>
#include <string>
#include <future>
using namespace std;

#include "Lock.h"
#include "Session.h"
#include "SendBuffer.h"