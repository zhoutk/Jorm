#include "Sqlit3Db.h"
#include <sstream>

Sqlit3Db::Sqlit3Db(const char* apFilename,
	const int aFlags,
	const int aBusyTimeoutMs,
	const char* apVfs) :
	mFilename(apFilename)
{
	sqlite3* handle;
	const int ret = sqlite3_open_v2(apFilename, &handle, aFlags, apVfs);
	mSQLitePtr.reset(handle);
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
	string querySql = "select ";// +" from users where id > 0";
	if (fields.empty()) {
		querySql.append("* ");
	}
	else {
		std::stringstream ss;
		for (size_t i = 0; i < fields.size(); ++i)
		{
			if (i != 0)
				ss << ",";
			ss << fields[i];
		}
		querySql.append(ss.str());
	}
	querySql.append(" from users where id > 0");
	Document rs = ExecQuerySql(querySql, fields);
	return rs;
}

Document Sqlit3Db::ExecQuerySql(string aQuery, vector<string> fields) {
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

		int nCol = fields.size();
		if (fields.empty()) {
			int insertPot = aQuery.find("where");
			insertPot = insertPot >= 0 ? insertPot : aQuery.length();
			string aQueryLimit0 = aQuery.substr(0, insertPot).append(" limit 0");
			char** pRes = NULL;
			int nRow = 0;
			char* pErr = NULL;
			sqlite3_get_table(handle, aQuery.c_str(), &pRes, &nRow, &nCol, &pErr);
			for (int j = 0; j < nCol; j++)
			{
				fields.push_back(*(pRes + j));
			}
			if (pErr != NULL)
			{
				sqlite3_free(pErr);
			}
			sqlite3_free_table(pRes);
		}
		
		Value arr(kArrayType);
		//Document::AllocatorType& allocator = rs.GetAllocator();
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			Value al(kObjectType);
			for (int j = 0; j < nCol; j++)
			{
				string v = (char *) sqlite3_column_text(stmt, j);
				al.AddMember(StringRef(fields.at(j).c_str()), Value(v.c_str(), rs.GetAllocator()), rs.GetAllocator());
			}
			arr.PushBack(al, rs.GetAllocator());
		}
		rs.AddMember("data", arr, rs.GetAllocator());
	}
	StringBuffer strBuffer;
	Writer<StringBuffer> writer(strBuffer);
	rs.Accept(writer);

	rs.Parse(strBuffer.GetString());

	//cout << "the result is : " << strBuffer.GetString() << endl;
	return rs;
}

void Sqlit3Db::Deleter::operator()(sqlite3* apSQLite)
{
	const int ret = sqlite3_close(apSQLite); // Calling sqlite3_close() with a nullptr argument is a harmless no-op.

	// Avoid unreferenced variable warning when build in release mode
	(void)ret;

	// Only case of error is SQLITE_BUSY: "database is locked" (some statements are not finalized)
	// Never throw an exception in a destructor :
	SQLITECPP_ASSERT(SQLITE_OK == ret, "database is locked");  // See SQLITECPP_ENABLE_ASSERT_HANDLER
}

