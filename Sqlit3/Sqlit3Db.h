#pragma once
#include "../common/Idb/Idb.h"
#include "../thirds/sqlit3/sqlite3.h"
#include "../common/Utils/Utils.h"
#include "../common/Utils/GlobalConstants.h"
#include <algorithm>

using namespace std;

namespace Sqlit3 {

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
		vector<string> QUERY_EXTRA_KEYS;
		vector<string> QUERY_UNEQ_OPERS;

	public:
		Sqlit3Db(const char* apFilename,
			const int   aFlags = OPEN_READWRITE | OPEN_CREATE,
			const int   aBusyTimeoutMs = 0,
			const char* apVfs = nullptr) : mFilename(apFilename)
		{
			QUERY_EXTRA_KEYS = Utils::MakeVectorInitFromString("ins,lks,ors");

			QUERY_UNEQ_OPERS.push_back(">,");
			QUERY_UNEQ_OPERS.push_back(">=,");
			QUERY_UNEQ_OPERS.push_back("<,");
			QUERY_UNEQ_OPERS.push_back("<=,");
			QUERY_UNEQ_OPERS.push_back("<>,");
			QUERY_UNEQ_OPERS.push_back("=,");

			sqlite3* handle;
			const int ret = sqlite3_open_v2(apFilename, &handle, aFlags, apVfs);
			mSQLitePtr.reset(handle);
			if (SQLITE_OK != ret)
			{
				string errmsg = "DB Error, code: ";
				errmsg.append(Utils::IntTransToString(ret)).append("; message: ");
				errmsg.append(sqlite3_errmsg(getHandle()));
				throw errmsg;
			}
			if (aBusyTimeoutMs > 0)
			{
				const int ret = sqlite3_busy_timeout(getHandle(), aBusyTimeoutMs);
				if (OK != ret)
				{
					string errmsg = "DB Error, code: ";
					errmsg.append(Utils::IntTransToString(ret)).append("; message: ");
					errmsg.append(sqlite3_errmsg(getHandle()));
					throw errmsg;
				}
			}
		};

		Sqlit3Db(const std::string& aFilename,
			const int          aFlags = OPEN_READWRITE | OPEN_CREATE,
			const int          aBusyTimeoutMs = 0,
			const std::string& aVfs = "") {
			new (this)Sqlit3Db(aFilename.c_str(), aFlags, aBusyTimeoutMs, aVfs.empty() ? nullptr : aVfs.c_str());
		};

		Rjson remove(string tablename, Rjson& params)
		{
			if (params.IsObject()) {
				string execSql = "delete from ";
				execSql.append(tablename).append(" where id = ");

				string v;
				int vType;
				params.GetValueAndTypeByKey("id", &v, &vType);

				if (vType == 6)
					execSql.append(v);
				else
					execSql.append("'").append(v).append("'");

				return ExecNoneQuerySql(execSql);
			}
			else {
				return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
			}
		}

		Rjson update(string tablename, Rjson& params)
		{
			if (params.IsObject()) {
				string execSql = "update ";
				execSql.append(tablename).append(" set ");

				vector<string> allKeys = params.GetAllKeys();

				vector<string>::iterator iter = find(allKeys.begin(), allKeys.end(), "id");
				if (iter == allKeys.end()) {
					return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
				}
				else {
					size_t len = allKeys.size();
					size_t conditionLen = len - 2;
					string fields = "", where = " where id = ";
					for (size_t i = 0; i < len; i++) {
						string key = allKeys[i];
						string v;
						int vType;
						params.GetValueAndTypeByKey(key, &v, &vType);
						if (key.compare("id") == 0) {
							conditionLen++;
							if (vType == 6)
								where.append(v);
							else
								where.append("'").append(v).append("'");
						}
						else {
							fields.append(key).append(" = ");
							if (vType == 6)
								fields.append(v);
							else
								fields.append("'").append(v).append("'");
							if (i < conditionLen) {
								fields.append(",");
							}
						}
					}
					execSql.append(fields).append(where);
					return ExecNoneQuerySql(execSql);
				}
			}
			else {
				return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
			}
		}

		Rjson create(string tablename, Rjson& params)
		{
			if (params.IsObject()) {
				string execSql = "insert into ";
				execSql.append(tablename).append(" ");

				vector<string> allKeys = params.GetAllKeys();
				size_t len = allKeys.size();
				string fields = "", vs = "";
				for (size_t i = 0; i < len; i++) {
					string key = allKeys[i];
					fields.append(key);
					string v;
					int vType;
					params.GetValueAndTypeByKey(key, &v, &vType);
					if (vType == 6)
						vs.append(v);
					else
						vs.append("'").append(v).append("'");
					if (i < len - 1) {
						fields.append(",");
						vs.append(",");
					}
				}
				execSql.append("(").append(fields).append(") values (").append(vs).append(")");
				return ExecNoneQuerySql(execSql);
			}
			else {
				return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
			}
		}

		Rjson execSql(string sql) {
			return ExecNoneQuerySql(sql);
		}

		Rjson querySql(string sql, Rjson& params = Rjson(), vector<string> filelds = vector<string>()) {
			return select(sql, params, filelds, 2);
		}

		Rjson insertBatch(string tablename, vector<Rjson> elements, string constraint) {
			string sql = "insert into ";
			if (elements.empty()) {
				return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
			}
			else {
				string keyStr = " (";
				keyStr.append(Utils::GetVectorJoinStr(elements[0].GetAllKeys())).append(" ) ");
				for (size_t i = 0; i < elements.size(); i++) {
					vector<string> keys = elements[i].GetAllKeys();
					string valueStr = " select ";
					for (size_t j = 0; j < keys.size(); j++) {
						valueStr.append("'").append(elements[i][keys[j]]).append("'");
						if (j < keys.size() - 1) {
							valueStr.append(",");
						}
					}
					if (i < elements.size() - 1) {
						valueStr.append(" union all ");
					}
					keyStr.append(valueStr);
				}
				sql.append(tablename).append(keyStr);
			}
			return ExecNoneQuerySql(sql);
		}

		Rjson transGo(vector<string> sqls, bool isAsync = false) {
			if (sqls.empty()) {
				return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
			}
			else {
				char* zErrMsg = 0;
				bool isExecSuccess = true;
				sqlite3_exec(getHandle(), "begin;", 0, 0, &zErrMsg);
				for (size_t i = 0; i < sqls.size(); i++) {
					string u8Query = Utils::UnicodeToU8(sqls[i]);
					int rc = sqlite3_exec(getHandle(), u8Query.c_str(), 0, 0, &zErrMsg);
					if (rc != SQLITE_OK)
					{
						isExecSuccess = false;
						cout << "Transaction Fail, sql " << i + 1 << " is wrong. Error: " << zErrMsg << endl;
						sqlite3_free(zErrMsg);
						break;
					}
				}
				if (isExecSuccess)
				{
					sqlite3_exec(getHandle(), "commit;", 0, 0, 0);
					sqlite3_close(getHandle());
					cout << "Transaction Success: run " << sqls.size() << " sqls." << endl;
					return Utils::MakeJsonObjectForFuncReturn(STSUCCESS, "Transaction success.");
				}
				else
				{
					sqlite3_exec(getHandle(), "rollback;", 0, 0, 0);
					sqlite3_close(getHandle());
					return Utils::MakeJsonObjectForFuncReturn(STDBOPERATEERR, zErrMsg);
				}
			}
		}

		Rjson select(string tablename, Rjson& params, vector<string> fields = vector<string>(), int queryType = 1) {
			if (params.IsObject()) {
				string querySql = "";
				string where = "";
				const string AndJoinStr = " and ";
				string fieldsJoinStr = "*";

				if (!fields.empty()) {
					fieldsJoinStr = Utils::GetVectorJoinStr(fields);
				}

				string fuzzy = params.GetStringValueAndRemove("fuzzy");
				string sort = params.GetStringValueAndRemove("sort");
				int page = atoi(params.GetStringValueAndRemove("page").c_str());
				int size = atoi(params.GetStringValueAndRemove("size").c_str());
				string sum = params.GetStringValueAndRemove("sum");
				string count = params.GetStringValueAndRemove("count");
				string group = params.GetStringValueAndRemove("group");

				vector<string> allKeys = params.GetAllKeys();
				size_t len = allKeys.size();
				for (size_t i = 0; i < len; i++) {
					string k = allKeys[i];
					string v;
					int vType;
					params.GetValueAndTypeByKey(k, &v, &vType);
					if (where.length() > 0) {
						where.append(AndJoinStr);
					}

					if (Utils::FindStringFromVector(QUERY_EXTRA_KEYS, k)) {   // process key
						string whereExtra = "";
						vector<string> ele = Utils::MakeVectorInitFromString(params[k]);
						if (ele.size() < 2 || ((k.compare("ors") == 0 || k.compare("lks") == 0) && ele.size() % 2 == 1)) {
							return Utils::MakeJsonObjectForFuncReturn(STPARAMERR, k + " is wrong.");
						}
						else {
							if (k.compare("ins") == 0) {
								string c = ele.at(0);
								vector<string>(ele.begin() + 1, ele.end()).swap(ele);
								whereExtra.append(c).append(" in ( ").append(Utils::GetVectorJoinStr(ele)).append(" )");
							}
							else if (k.compare("lks") == 0 || k.compare("ors") == 0) {
								whereExtra.append(" ( ");
								for (size_t j = 0; j < ele.size(); j += 2) {
									if (j > 0) {
										whereExtra.append(" or ");
									}
									whereExtra.append(ele.at(j)).append(" ");
									string eqStr = k.compare("lks") == 0 ? " like '" : " = '";
									string vsStr = ele.at(j + 1);
									if (k.compare("lks") == 0) {
										vsStr.insert(0, "%");
										vsStr.append("%");
									}
									vsStr.append("'");
									whereExtra.append(eqStr).append(vsStr);
								}
								whereExtra.append(" ) ");
							}
						}
						where.append(whereExtra);
					}
					else {				// process value
						if (Utils::FindStartsStringFromVector(QUERY_UNEQ_OPERS, (char*)v.c_str())) {
							vector<string> vls = Utils::MakeVectorInitFromString(v);
							if (vls.size() == 2) {
								where.append(k).append(vls.at(0)).append("'").append(vls.at(1)).append("'");
							}
							else if (vls.size() == 4) {
								where.append(k).append(vls.at(0)).append("'").append(vls.at(1)).append("' and ");
								where.append(k).append(vls.at(2)).append("'").append(vls.at(3)).append("'");
							}
							else {
								return Utils::MakeJsonObjectForFuncReturn(STPARAMERR, "not equal value is wrong.");
							}
						}
						else if (!fuzzy.empty() && vType == kStringType) {
							where.append(k).append(" like '%").append(v).append("%'");
						}
						else {
							if (vType == kNumberType)
								where.append(k).append(" = ").append(v);
							else
								where.append(k).append(" = '").append(v).append("'");
						}
					}
				}

				string extra = "";
				if (!sum.empty()) {
					vector<string> ele = Utils::MakeVectorInitFromString(sum);
					if (ele.empty() || ele.size() % 2 == 1)
						return Utils::MakeJsonObjectForFuncReturn(STPARAMERR, "sum is wrong.");
					else {
						for (size_t i = 0; i < ele.size(); i += 2) {
							extra.append(",sum(").append(ele.at(i)).append(") as ").append(ele.at(i + 1)).append(" ");
						}
					}
				}
				if (!count.empty()) {
					vector<string> ele = Utils::MakeVectorInitFromString(count);
					if (ele.empty() || ele.size() % 2 == 1)
						return Utils::MakeJsonObjectForFuncReturn(STPARAMERR, "count is wrong.");
					else {
						for (size_t i = 0; i < ele.size(); i += 2) {
							extra.append(",count(").append(ele.at(i)).append(") as ").append(ele.at(i + 1)).append(" ");
						}
					}
				}

				if (queryType == 1) {
					querySql.append("select ").append(fieldsJoinStr).append(extra).append(" from ").append(tablename);
					if (where.length() > 0)
						querySql.append(" where ").append(where);
				}
				else {
					querySql.append(tablename);
					if (!fields.empty()) {
						size_t starIndex = querySql.find('*');
						if (starIndex < 10) {
							querySql.replace(starIndex, 1, fieldsJoinStr.c_str());
						}
					}
					if (where.length() > 0) {
						size_t whereIndex = querySql.find("where");
						if (whereIndex == querySql.npos) {
							querySql.append(" where ").append(where);
						}
						else {
							querySql.append(" and ").append(where);
						}
					}
				}

				if (!group.empty()) {
					querySql.append(" group by ").append(group);
				}

				if (!sort.empty()) {
					querySql.append(" order by ").append(sort);
				}

				if (page > 0) {
					page--;
					querySql.append(" limit ").append(Utils::IntTransToString(page * size)).append(",").append(Utils::IntTransToString(size));
				}

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

	private:
		Rjson ExecQuerySql(string aQuery, vector<string> fields) {
			Rjson rs = Utils::MakeJsonObjectForFuncReturn(STSUCCESS);
			sqlite3_stmt* stmt = NULL;
			sqlite3* handle = getHandle();
			string u8Query = Utils::UnicodeToU8(aQuery);
			const int ret = sqlite3_prepare_v2(handle, u8Query.c_str(), static_cast<int>(u8Query.size()), &stmt, NULL);
			if (SQLITE_OK != ret)
			{
				string errmsg = sqlite3_errmsg(getHandle());
				rs.ExtendObject(Utils::MakeJsonObjectForFuncReturn(STDBOPERATEERR, errmsg));
			}
			else {
				int insertPot = aQuery.find("where");
				if (insertPot == aQuery.npos) {
					insertPot = aQuery.find("limit");
					if (insertPot == aQuery.npos) {
						insertPot = aQuery.length();
					}
				}
				string aQueryLimit0 = aQuery.substr(0, insertPot).append(" limit 1");
				char** pRes = NULL;
				int nRow = 0, nCol = 0;
				char* pErr = NULL;
				sqlite3_get_table(handle, aQueryLimit0.c_str(), &pRes, &nRow, &nCol, &pErr);
				for (int j = 0; j < nCol; j++)
				{
					string fs = *(pRes + j);
					if (find(fields.begin(), fields.end(), fs) == fields.end()) {
						fields.push_back(fs);
					}
				}
				if (pErr != NULL)
				{
					sqlite3_free(pErr);
				}
				sqlite3_free_table(pRes);

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
							al.AddValueFloat(k, sqlite3_column_double(stmt, j));
						}
						else if (nType == 3) {				//SQLITE_TEXT
							al.AddValueString(k, Utils::U8ToUnicode((char*)sqlite3_column_text(stmt, j)));
						}
						//else if (nType == 4) {				//SQLITE_BLOB

						//}
						//else if (nType == 5) {				//SQLITE_NULL

						//}
						else{
							al.AddValueString(k, "");
						}
					}
					arr.push_back(al);
				}
				if (arr.empty())
					rs.ExtendObject(Utils::MakeJsonObjectForFuncReturn(STQUERYEMPTY));
				rs.AddValueObjectArray("data", arr);
			}
			sqlite3_finalize(stmt);
			cout << "SQL: " << aQuery << endl;
			return rs;
		}

		Rjson ExecNoneQuerySql(string aQuery) {
			Rjson rs = Utils::MakeJsonObjectForFuncReturn(STSUCCESS);
			sqlite3_stmt* stmt = NULL;
			sqlite3* handle = getHandle();
			string u8Query = Utils::UnicodeToU8(aQuery);
			const int ret = sqlite3_prepare_v2(handle, u8Query.c_str(), static_cast<int>(u8Query.size()), &stmt, NULL);
			if (SQLITE_OK != ret)
			{
				string errmsg = sqlite3_errmsg(getHandle());
				rs.ExtendObject(Utils::MakeJsonObjectForFuncReturn(STDBOPERATEERR, errmsg));
			}
			else {
				sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			cout << "SQL: " << aQuery << endl;
			return rs;
		}

	};
}
