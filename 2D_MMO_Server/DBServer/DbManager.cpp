#include "pch.h"
#include "DbManager.h"
#include "PoolManager.h"
#include "ThreadPool.h"
#include "format.h"
#include <Service.h>

DbManager* DbManager::_instance = nullptr;

DbManager& DbManager::Instance()
{
	if (_instance == nullptr)
		_instance = new DbManager();
	return *_instance;
}

DbManager::DbManager() {}

void DbManager::Init(json& j)
{
	auto ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_henv);
	ASSERT(SQL_SUCCEEDED(ret), "");
	ret = SQLSetEnvAttr(_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	ASSERT(SQL_SUCCEEDED(ret), "");

	string ip = j["ip"];
	string dbName = j["db_name"];
	string uid = j["uid"];
	string password = j["password"];
	string driver = j["driver"];

	string connectionString = Utils::format((string)CONNECTION_STRING, { driver, ip, dbName, uid, password });

	wstring connectionStringW;
	connectionStringW.assign(connectionString.begin(), connectionString.end());
	const wchar_t* str = connectionStringW.c_str();

	
	// 환경 핸들 설정 및 sql 서버 접속
	{
		GPoolManager->CreatePool<SQLHDBC>(OPERATION_NUMBER, OPERATION_NUMBER);
		vector<SQLHDBC*> dbcVec;
		dbcVec.reserve(OPERATION_NUMBER);
		for (int32 i = 0; i < OPERATION_NUMBER; i++)
		{
			SQLHDBC* dbc = GPoolManager->Pop<SQLHDBC>();

			ret = SQLAllocHandle(SQL_HANDLE_DBC, _henv, dbc);
			ASSERT(SQL_SUCCEEDED(ret), "");
			ret = SQLDriverConnect(*dbc, NULL, (SQLTCHAR*)str, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
			ASSERT(SQL_SUCCEEDED(ret), "");
			dbcVec.push_back(dbc);
		}
		for (SQLHDBC* dbc : dbcVec)
		{
			GPoolManager->Push<SQLHDBC>(dbc);
		}
	}
}

void DbManager::RequestAsync(wstring req, function<void(QUERY_REF)> callback)
{
	GThreadManager->EnqueueJob([req, this, callback]() {
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
		auto func = queryArgs->GetCallBack();
		if (func != nullptr)
			func(queryArgs);
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
