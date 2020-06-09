#include "Sqlit3Db.h"

Sqlit3Db::Sqlit3Db(const char* apFilename,
	const int aFlags,
	const int aBusyTimeoutMs,
	const char* apVfs) :
	mFilename(apFilename)
{
	sqlite3* handle;
	const int ret = sqlite3_open_v2(apFilename, &mSQLitePtr, aFlags, apVfs);
	//this->mSQLitePtr = handle;
	if (SQLITE_OK != ret)
	{
		throw "SQLite Error, return code: " + ret;
	}
	if (aBusyTimeoutMs > 0)
	{
		const int ret = sqlite3_busy_timeout(getHandle(), aBusyTimeoutMs);
		check(ret);
	}
}

Sqlit3Db::~Sqlit3Db()
{
}

Document Sqlit3Db::retrieve(string tablename, Document* params, vector<string> fields)
{
	sqlite3* db = getHandle();
	string querySql = "select * from users";
	Document rs = ExecQuerySql(querySql);
	return rs;
}

Document Sqlit3Db::ExecQuerySql(string aQuery) {
	Document rs;
	rs.Parse("{}");
	sqlite3_stmt* stmt = NULL;
	sqlite3* handle = getHandle();
	const int ret = sqlite3_prepare_v2(handle, aQuery.c_str(), static_cast<int>(aQuery.size()), &stmt, NULL);
	if (SQLITE_OK != ret)
	{
		Value code("801");
		rs.AddMember("code", code, rs.GetAllocator());
	}
	else {
		Value code("200");
		rs.AddMember("code", code, rs.GetAllocator());
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			const unsigned char* username = sqlite3_column_text(stmt, 0);
			const unsigned char* password = sqlite3_column_text(stmt, 1);
			std::clog << "username = " << username << ", password = " << password;
		}
	}
	return rs;
}

//void Sqlit3Db::Deleter::operator()(sqlite3* apSQLite)
//{
//	const int ret = sqlite3_close(apSQLite); // Calling sqlite3_close() with a nullptr argument is a harmless no-op.
//
//	// Avoid unreferenced variable warning when build in release mode
//	(void)ret;
//
//	// Only case of error is SQLITE_BUSY: "database is locked" (some statements are not finalized)
//	// Never throw an exception in a destructor :
//	SQLITECPP_ASSERT(SQLITE_OK == ret, "database is locked");  // See SQLITECPP_ENABLE_ASSERT_HANDLER
//}

