#include "pch.h"
#include "DbManager.h"
#include "PoolManager.h"
#include "ThreadPool.h"
#include "format.h"

DbManager* DbManager::_instance = nullptr;

DbManager& DbManager::Instance()
{
	if (_instance == nullptr)
		_instance = new DbManager();
	return *_instance;
}

DbManager::DbManager() {}

void DbManager::WaitForEvents()
{
#if defined(ASYNC)
	{
		while (true)
		{
			this_thread::sleep_for(0.1s);
			vector<HANDLE> handles;
			vector<QUERY_REF> querys;

			{
				READ_LOCK;
				for (auto& query : _querys) {
					querys.push_back(query);
					handles.push_back(query->GetHandle());
				}
			}

			if (handles.empty())
				continue;

			DWORD waitResult = WaitForMultipleObjects(
				handles.size(),
				handles.data(),
				false,
				1000
			);

			if (waitResult == WAIT_TIMEOUT)
				continue;
			if (waitResult == WAIT_FAILED)
			{
				auto errCode = GetLastError();

				cerr << "WaitForMultipleObjects Failed" << errCode << "\n";

				// todo log

				continue;
			}
			auto completedQuery = querys[waitResult - WAIT_OBJECT_0];
			RETCODE retCode;
			auto ret = SQLCompleteAsync(SQL_HANDLE_STMT, completedQuery->GetStmt(), &retCode);
			if (SQL_SUCCEEDED(ret) && SQL_SUCCEEDED(retCode))
			{
				auto func = completedQuery->GetCallBack();

				if (func != nullptr) {
					WRITE_LOCK;
					_jobQueue.push([func, completedQuery]() { func(completedQuery.get()); });
					_querys.erase(_querys.begin() + (waitResult - WAIT_OBJECT_0));
				}
				else
				{
					goto ERR;
				}
			}
			else
			{
				goto ERR;
			}
			continue;
		ERR:
			// todo
			cerr << "������ ������ ��Ȳ �߻�\n";
		}
	}
#endif

}

void DbManager::Init(json& j)
{
	auto ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_henv);
	ASSERT_CRASH(SQL_SUCCEEDED(ret));
	ret = SQLSetEnvAttr(_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	ASSERT_CRASH(SQL_SUCCEEDED(ret));

	string ip = j["ip"];
	string dbName = j["db_name"];
	string uid = j["uid"];
	string password = j["password"];
	string driver = j["driver"];

	string connectionString = Utils::format((string)CONNECTION_STRING, { driver, ip, dbName, uid, password });

	wstring connectionStringW;
	connectionStringW.assign(connectionString.begin(), connectionString.end());
	const wchar_t* str = connectionStringW.c_str();

	// ȯ�� �ڵ� ���� �� sql ���� ����
	{
		GPoolManager->CreatePool<SQLHDBC>(OPERATION_NUMBER, OPERATION_NUMBER);
		vector<SQLHDBC*> dbcVec;
		dbcVec.reserve(OPERATION_NUMBER);
		for (int32 i = 0; i < OPERATION_NUMBER; i++)
		{
			SQLHDBC* dbc = GPoolManager->Pop<SQLHDBC>();

			ret = SQLAllocHandle(SQL_HANDLE_DBC, _henv, dbc);
			ASSERT_CRASH(SQL_SUCCEEDED(ret));
			ret = SQLDriverConnect(*dbc, NULL, (SQLTCHAR*)str, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
			ASSERT_CRASH(SQL_SUCCEEDED(ret));
			dbcVec.push_back(dbc);
		}
		for (SQLHDBC* dbc : dbcVec)
		{
			GPoolManager->Push<SQLHDBC>(dbc);
		}
	}

#if defined(ASYNC)
	GThreadPool->EnqueueJob([&]() { WaitForEvents(); });
#elif defined(SYNC)
#endif
}

void DbManager::RequestAsync(wstring req, function<void(QUERY_REF)> callback)
{
	GThreadPool->EnqueueJob([req, this, callback]() {
		wcout << "req : " << req << endl;
		STMT_REF stmt(new SQLHSTMT(),
			[](SQLHSTMT* stmt) {
				SQLFreeHandle(SQL_HANDLE_STMT, *stmt);
				delete stmt;
			});

		SQLHDBC* hdbc = nullptr;
		while (hdbc == nullptr)
			hdbc = GPoolManager->Pop<SQLHDBC>();

		auto ret = SQLAllocHandle(SQL_HANDLE_STMT, *hdbc, stmt.get());
		if (SQL_SUCCEEDED(ret) == false)
		{
			cerr << "Sql Query Gen Failed\n";
			HandleError(SQL_HANDLE_DBC, hdbc);
			return;
		}

		HANDLE_REF hEvent(new HANDLE(CreateEvent(nullptr, TRUE, FALSE, nullptr)), [](HANDLE* hEvent) {
			CloseHandle(*hEvent);
			delete hEvent;
			});
		QUERY_REF queryArgs = make_shared<QueryArgs>(stmt, hEvent, hdbc, callback);
		queryArgs->_ret = SQLExecDirect(queryArgs->GetStmt(), (SQLWCHAR*)req.c_str(), SQL_NTS);
#if defined(ASYNC)
		ret = SQLSetStmtAttr(*stmt.get(), SQL_ATTR_ASYNC_ENABLE, (SQLPOINTER)SQL_ASYNC_ENABLE_ON, SQL_IS_INTEGER);
		if (SQL_SUCCEEDED(ret) == false)
		{
			cerr << "SQLSetStmtAttr Failed\n";
			return;
		}

		ret = SQLSetStmtAttr(*stmt.get(), SQL_ATTR_ASYNC_STMT_EVENT, *hEvent.get(), SQL_IS_POINTER);
		if (SQL_SUCCEEDED(ret) == false)
		{
			cerr << "SQLSetStmtAttr Failed\n";
			HandleError(hdbc);
			return;
		}
		if (queryArgs->_ret != SQL_STILL_EXECUTING) {
			cerr << "SQLExecDirect Failed\n";
			return;
		}
		WRITE_LOCK;
		{
			_querys.push_back(queryArgs);
		}
#elif defined(SYNC)
		auto func = queryArgs->GetCallBack();
		if (func != nullptr)
			func(queryArgs);
#endif
		});
}

void DbManager::RequestAsync(wstring cmd, wstring table, wstring condition, wstring order, function<void(QUERY_REF)> callback)
{
	wstring req = cmd + L" " + table + L" " + condition + L" " + order + L";";
	RequestAsync(req, callback);
}

void DbManager::HandleError(SQLSMALLINT htype, SQLHANDLE* dbc)
{
	SQLWCHAR sql_state[6] = {}, message[1024] = {};
	SQLSMALLINT msg_len = 0;
	SQLINTEGER native_error = 0;
	auto ret = SQLGetDiagRec(htype, *dbc, 1, sql_state, &native_error, message, sizeof(message) / sizeof(wchar_t), &msg_len);
	wcout << "state : " << sql_state << ", message : " << message << endl;
}

DbManager::QueryArgs::QueryArgs(STMT_REF stmt, HANDLE_REF hEvent, SQLHDBC* hdbc, function<void(QUERY_REF)> callback)
{
	_stmt = move(stmt);
	_hEvent = move(hEvent);
	_hdbc = hdbc;
	_callback = move(callback);
}
