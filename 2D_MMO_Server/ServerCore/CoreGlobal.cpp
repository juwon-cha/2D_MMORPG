#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadPool.h"
#include "SocketUtils.h"
#include "PoolManager.h"

ThreadPool* GThreadManager = nullptr;
PoolManager* GPoolManager = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadPool(8);
		GPoolManager = new PoolManager();
		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		SocketUtils::Clear();
	}
} GCoreGlobal;
