#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "SocketUtils.h"
#include "PoolManager.h"

ThreadManager* GThreadManager = nullptr;
PoolManager* GPoolManager = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GPoolManager = new PoolManager();
		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		SocketUtils::Clear();
	}
} GCoreGlobal;
