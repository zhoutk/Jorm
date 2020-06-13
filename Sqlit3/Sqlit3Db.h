#pragma once
#include "Idb.h"
#include "sqlit3/sqlite3.h"
#include "Utils.h"
#include "GlobalConstants.h"
#include <sstream>

using namespace std;

#define SQLITECPP_ASSERT(expression, message)   assert(expression && message)

const int   OPEN_READONLY = SQLITE_OPEN_READONLY;
const int   OPEN_READWRITE = SQLITE_OPEN_READWRITE;
const int   OPEN_CREATE = SQLITE_OPEN_CREATE;
const int   OPEN_URI = SQLITE_OPEN_URI;

const int   OK = SQLITE_OK;

class Sqlit3Db : public Idb
{
public:
	struct Deleter
	{
		void operator()(sqlite3* apSQLite) {
			const int ret = sqlite3_close(apSQLite);
			(void)ret;
			SQLITECPP_ASSERT(SQLITE_OK == ret, "database is locked");
		};
	};

private:
	std::unique_ptr<sqlite3, Deleter> mSQLitePtr;
	std::string mFilename;

public:
	Sqlit3Db(const char* apFilename,
		const int   aFlags = OPEN_READWRITE,
		const int   aBusyTimeoutMs = 0,
		const char* apVfs = nullptr) : mFilename(apFilename)
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
	};

	Sqlit3Db(const std::string& aFilename,
		const int          aFlags = OPEN_READWRITE,
		const int          aBusyTimeoutMs = 0,
		const std::string& aVfs = "") {
		new (this)Sqlit3Db(aFilename.c_str(), aFlags, aBusyTimeoutMs, aVfs.empty() ? nullptr : aVfs.c_str());
	};

	Rjson retrieve(string tablename, Rjson& params, vector<string> fields = vector<string>()) {
		if (params.IsObject()) {
			string querySql = "select ";
			string where = "";
			const string AndJoinStr = " and ";

			if (fields.empty()) {
				querySql.append("*");
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
			querySql.append(" from ").append(tablename);

			vector<string> allKeys = params.GetAllKeys();
			int len = allKeys.size();
			for (int i = 0; i < len; i++) {
				string k = allKeys[i];
				string v;
				bool v_number;
				params.GetValueAndTypeByKey(k, &v, &v_number);
				if (where.length() > 0) {
					where.append(AndJoinStr);
				}

				if (v_number)
					where.append(k).append(" = ").append(v);
				else
					where.append(k).append(" = '").append(v).append("'");
			}

			if (where.length() > 0)
				querySql.append(" where ").append(where);
			return ExecQuerySql(querySql, fields);
		}
		else {
			return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
		}
	};

	sqlite3* getHandle()
	{
		return mSQLitePtr.get();
	}

	void check(const int aRet)
	{
		if (OK != aRet)
		{
			throw "SQLite Error, return code: " + aRet;
		}
	}

private:
	Rjson ExecQuerySql(string aQuery, vector<string> fields) {
		Rjson rs = Utils::MakeJsonObjectForFuncReturn(STSUCCESS);
		sqlite3_stmt* stmt = NULL;
		sqlite3* handle = getHandle();
		string u8Query = Utils::UnicodeToU8(aQuery);
		const int ret = sqlite3_prepare_v2(handle, u8Query.c_str(), static_cast<int>(u8Query.size()), &stmt, NULL);
		if (SQLITE_OK != ret)
		{
			rs.ExtendObject(Utils::MakeJsonObjectForFuncReturn(STDBOPERATEERR));
		}
		else {
			int nCol = fields.size();
			if (fields.empty()) {
				int insertPot = aQuery.find("where");
				insertPot = insertPot >= 0 ? insertPot : aQuery.length();
				string aQueryLimit0 = aQuery.substr(0, insertPot).append(" limit 1");
				char** pRes = NULL;
				int nRow = 0;
				char* pErr = NULL;
				sqlite3_get_table(handle, aQueryLimit0.c_str(), &pRes, &nRow, &nCol, &pErr);
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

			vector<Rjson> arr;
			while (sqlite3_step(stmt) == SQLITE_ROW) {
				Rjson al;
				for (int j = 0; j < nCol; j++)
				{
					string k = fields.at(j);
					int nType = sqlite3_column_type(stmt, j);
					if (nType == 1) {					//SQLITE_INTEGER
						al.AddValueInt(k, sqlite3_column_int(stmt, j));
					}
					else if (nType == 2) {				//SQLITE_FLOAT

					}
					else if (nType == 3) {				//SQLITE_TEXT
						al.AddValueString(k, Utils::U8ToUnicode((char*)sqlite3_column_text(stmt, j)));
					}
					else if (nType == 4) {				//SQLITE_BLOB

					}
					else if (nType == 5) {				//SQLITE_NULL

					}
				}
				arr.push_back(Rjson(al));
			}
			rs.AddValueObjectArray("data", arr);
		}
		sqlite3_finalize(stmt);
		cout << "SQL: " << aQuery << endl;
		return rs;
	}

                     
};

