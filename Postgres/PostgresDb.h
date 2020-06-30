#pragma once

#include "../common/Idb/Idb.h"
#include "../common/Utils/Utils.h"
#include "../common/Utils/GlobalConstants.h"

#undef min
#undef max
#include "../thirds/postgres/pqxx/pqxx"
#undef NOMINMAX

using namespace std;
using namespace pqxx;

namespace Postgres {

	const int MAXCONN = 2;
	char* QUERY_EXTRA_KEYS[] = { "ins", "lks", "ors" };
	char* QUERY_UNEQ_OPERS[] = { ">,", ">=,", "<,", "<=,", "<>,", "=," };

	class PostgresDb : public Idb
	{

	private:
		connection* GetConnection(string* err = nullptr) {
			//srand((unsigned int)(time(NULL)));
			size_t index = (rand() % maxConn) + 1;
			if (index > pool.size()) {
				try {
					connection* pqsql;
					pqsql = new connection(connStr);
					if (pqsql->is_open())
					{
						pool.push_back(pqsql);
						return pqsql;
					}
					else
						return nullptr;
				}
				catch (const std::exception& e) {
					err = new string(e.what());
					return nullptr;
				}
			}
			else {
				return pool.at(index - 1);
			}
		}

	public:

		PostgresDb(string connStr) :connStr(connStr), maxConn(MAXCONN) {
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


		Rjson querySql(string sql, Rjson& params = Rjson(), vector<string> filelds = vector<string>())
		{
			return select(sql, params, filelds, 2);
		}


		Rjson execSql(string sql)
		{
			return ExecNoneQuerySql(sql);
		}

		Rjson insertBatch(string tablename, vector<Rjson> elements, string constraint)
		{
			string sql = "insert into ";
			vector<string> restrain = Utils::MakeVectorInitFromString(constraint);
			if (elements.empty()) {
				return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
			}
			else {
				string keyStr = " ( ";
				string updateStr = "";
				keyStr.append(Utils::GetVectorJoinStr(elements[0].GetAllKeys())).append(" ) values ");
				for (size_t i = 0; i < elements.size(); i++) {
					vector<string> keys = elements[i].GetAllKeys();
					string valueStr = " ( ";
					for (size_t j = 0; j < keys.size(); j++) {
						if (i == 0) {
							vector<string>::iterator iter = find(restrain.begin(), restrain.end(), keys[j]);
							if (iter == restrain.end())
								updateStr.append(keys[j]).append(" = excluded.").append(keys[j]).append(",");
						}
						valueStr.append("'").append(elements[i][keys[j]]).append("'");
						if (j < keys.size() - 1) {
							valueStr.append(",");
						}
					}
					valueStr.append(" )");
					if (i < elements.size() - 1) {
						valueStr.append(",");
					}
					keyStr.append(valueStr);
				}
				if (updateStr.length() == 0) {
					sql.append(tablename).append(keyStr);
				}
				else
				{
					updateStr = updateStr.substr(0, updateStr.length() - 1);
					sql.append(tablename).append(keyStr).append(" on conflict (").append(constraint).append(") do update set ").append(updateStr);
				}
			}
			return ExecNoneQuerySql(sql);
		}


		Rjson transGo(vector<string> sqls, bool isAsync = false)
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		Rjson select(string tablename, Rjson& params, vector<string> fields = vector<string>(), int queryType = 1)
		{
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

					if (Utils::FindCharArray(QUERY_EXTRA_KEYS, (char*)k.c_str())) {   // process key
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
						if (Utils::FindStartsCharArray(QUERY_UNEQ_OPERS, (char*)v.c_str())) {
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
		}





		~PostgresDb()
		{
			while (pool.size())
			{
				(pool.back())->disconnect();
				pool.pop_back();
			}
		}

	private:

		Rjson ExecQuerySql(string aQuery, vector<string> fields) {
			Rjson rs = Utils::MakeJsonObjectForFuncReturn(STSUCCESS);
			string u8Query = Utils::UnicodeToU8(aQuery);
			string err;
			connection* pq = GetConnection(&err);
			if (pq == nullptr)
				return Utils::MakeJsonObjectForFuncReturn(STDBCONNECTERR, Utils::U8ToUnicode((char*)err.c_str()));
			try {
				nontransaction N(*pq);
				result R(N.exec(u8Query));

				size_t coLen = R.columns();
				vector<Rjson> arr;
				for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
					Rjson al;
					for (int i = 0; i < coLen; ++i)
					{
						al.AddValueString(R.column_name(i), Utils::U8ToUnicode((char*)c[i].c_str()));
					}
					arr.push_back(al);
				}
				if (arr.empty())
					rs.ExtendObject(Utils::MakeJsonObjectForFuncReturn(STQUERYEMPTY));
				rs.AddValueObjectArray("data", arr);

				cout << "SQL: " << aQuery << endl;
				return rs;
			}
			catch (const std::exception& e) {
				return Utils::MakeJsonObjectForFuncReturn(STDBOPERATEERR, Utils::U8ToUnicode((char*)e.what()));
			}
		}

		Rjson ExecNoneQuerySql(string aQuery) {
			Rjson rs = Utils::MakeJsonObjectForFuncReturn(STSUCCESS);
			string u8Query = Utils::UnicodeToU8(aQuery);
			string err;
			connection* pq = GetConnection(&err);
			if (pq == nullptr)
				return Utils::MakeJsonObjectForFuncReturn(STDBCONNECTERR, Utils::U8ToUnicode((char*)err.c_str()));
			try {
				nontransaction N(*pq);
				result R(N.exec(u8Query));

				rs.AddValueInt("affected", R.affected_rows());
				rs.AddValueInt("newId", R.inserted_oid());
			}
			catch (const std::exception& e) {
				return Utils::MakeJsonObjectForFuncReturn(STDBOPERATEERR, Utils::U8ToUnicode((char*)e.what()));
			}
			cout << "SQL: " << aQuery << endl;
			return rs;
		}

	private:
		vector<connection*> pool;
		int maxConn;
		string connStr;
	};

}