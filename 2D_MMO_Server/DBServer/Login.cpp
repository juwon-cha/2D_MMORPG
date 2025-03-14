#include "pch.h"
#include "PacketHandler.h"
#include "format.h"

void HandleSignUp(PacketSession* session, const uint64& sessionId, const string& name)
{
	wstring query = Utils::wformat("select id from user_account where name='{0}'", { name });

	Manager::DB.RequestAsync(query, [sessionId](shared_ptr<DbManager::QueryArgs> result) {
		FlatBufferBuilder builder;
		SQLINTEGER dbId = 0;
		SQLHSTMT stmt = result->GetStmt();
		HANDLE handle = result->GetHandle();

		SQLINTEGER ret = 0;
		SQLLEN dbIdIndicator;
		SQLBindCol(stmt, 1, SQL_C_LONG, &dbId, sizeof(dbId), &dbIdIndicator);

		SQLRETURN fetchResult = SQLFetch(stmt);
		auto data = CreateD_SIGNUP(builder, dbId, sessionId, SignUpError_SUCCESS);
		auto pkt = Manager::Packet.CreatePacket(data, builder, PacketType_D_SIGNUP);
		Manager::session->Send(pkt);
		});
}

void PacketHandler::SD_SIGNUPHandler(PacketSession* session, ByteRef buffer) {
	auto pkt = GetRoot<SD_SIGNUP>(buffer->operator unsigned char* ());
	auto sessionId = pkt->session_id();
	auto name = pkt->id()->str();
	auto password = pkt->password()->str();

	try {
		wstring query = Utils::wformat(
			"insert into user_account(name, password) "
			"values('{0}', '{1}'); ", { name, password });

		Manager::DB.RequestAsync(query, [sessionId, session, name](shared_ptr<DbManager::QueryArgs> result) {
			if (SQL_SUCCEEDED(result->_ret))
			{
				HandleSignUp(session, sessionId, name);
			}
			else
			{
				FlatBufferBuilder builder;
				auto data = CreateD_SIGNUP(builder, 0, sessionId, SignUpError_OVERLAP_ID);
				auto pkt = Manager::Packet.CreatePacket(data, builder, PacketType_D_SIGNUP);
				Manager::session->Send(pkt);
			}
			});
	}
	catch (...)
	{
		FlatBufferBuilder builder;
		auto data = CreateD_SIGNUP(builder, 0, sessionId, SignUpError_UNKNOWN);
		auto pkt = Manager::Packet.CreatePacket(data, builder, PacketType_D_SIGNUP);
		Manager::session->Send(pkt);
	}
}
void PacketHandler::SD_SIGNINHandler(PacketSession* session, ByteRef buffer)
{
	auto pkt = GetRoot<SD_SIGNIN>(buffer->operator unsigned char* ());
	string id = pkt->id()->str();
	string password = pkt->password()->str();
	uint32 sessionId = pkt->session_id();

	wstring query = Utils::wformat(
		"with data as "
		"(select * from user_account where name='{0}') "
		"select id, case "
		"when name is null then 1 "	// 1 == INVALID_ID
		"when password != '{1}' then 2 "// 2 == INVALID_PW
		"else 0 "						// 0 == SUCCESS
		"end as result "
		"from data", { id, password });

	try {
		Manager::DB.RequestAsync(query, [sessionId](shared_ptr<DbManager::QueryArgs> result) {
			SignInError error;
			SQLINTEGER dbId = 0;

			SQLHSTMT stmt = result->GetStmt();
			if (SQL_SUCCEEDED(result->_ret))
			{
				HANDLE handle = result->GetHandle();

				SQLBIGINT ret = 0;
				SQLLEN dbIdIndicator;
				SQLLEN retIndicator;
				SQLBindCol(stmt, 1, SQL_C_SBIGINT, &dbId, sizeof(dbId), &dbIdIndicator);
				SQLBindCol(stmt, 2, SQL_C_LONG, &ret, sizeof(ret), &retIndicator);

				SQLRETURN fetchResult = SQLFetch(stmt);
				if (fetchResult == SQL_NO_DATA)
					error = SignInError_INVALID_ID;
				else if (fetchResult == SQL_SUCCESS)
					error = (SignInError)ret;
			}
			else
			{
				error = SignInError_UNKNOWN;
				Manager::DB.HandleError(SQL_HANDLE_STMT, (SQLHANDLE*)&stmt);
			}
			FlatBufferBuilder builder;
			auto data = CreateD_SIGNIN(builder, dbId, sessionId, error);
			auto pkt = Manager::Packet.CreatePacket(data, builder, PacketType_D_SIGNIN);
			Manager::session->Send(pkt);
			});
	}
	catch (exception& e)
	{
		FlatBufferBuilder builder;
		auto data = CreateD_SIGNIN(builder, pkt->session_id(), SignInError_UNKNOWN);
		auto bytes = Manager::Packet.CreatePacket(data, builder, PacketType_D_SIGNIN);
		Manager::session->Send(bytes);
	}
}
