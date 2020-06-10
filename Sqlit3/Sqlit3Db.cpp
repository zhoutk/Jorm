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

Document Sqlit3Db::retrieve(string tablename, rapidjson::Document* params, vector<string> fields)
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
	querySql.append(" from ").append(tablename).append(" ");

	bool flag = (*params).IsObject();

	for (auto iter = (*params).MemberBegin(); iter != (*params).MemberEnd(); ++iter)
	{
		string k = (iter->name).GetString();
		string v = (iter->value).GetString();
		int dd = 2222;
	}

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
				string* k = new string(fields.at(j));				//create a new string, because fields will be detached.
				int nType = sqlite3_column_type(stmt, j);
				if (nType == 1) {					//SQLITE_INTEGER
					Value v(kNumberType);
					v.SetInt(sqlite3_column_int(stmt, j));
					al.AddMember(StringRef((*k).c_str()), v, rs.GetAllocator());
				}
				else if (nType == 2) {				//SQLITE_FLOAT
					
				}
				else if (nType == 3) {				//SQLITE_TEXT
					Value v(kStringType);
					v.SetString(Utils::U8ToUnicode((char*)sqlite3_column_text(stmt, j)), rs.GetAllocator());
					al.AddMember(StringRef((*k).c_str()), v, rs.GetAllocator());
				}
				else if (nType == 4) {				//SQLITE_BLOB

				}
				else if (nType == 5) {				//SQLITE_NULL

				}
			}
			arr.PushBack(al, rs.GetAllocator());
		}
		sqlite3_finalize(stmt);
		rs.AddMember("data", arr, rs.GetAllocator());
	}
	cout << "SQL: " << aQuery << endl;
	return rs;
}

void Sqlit3Db::Deleter::operator()(sqlite3* apSQLite)
{
	const int ret = sqlite3_close(apSQLite);
	(void)ret;
	SQLITECPP_ASSERT(SQLITE_OK == ret, "database is locked"); 
}

