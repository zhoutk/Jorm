#pragma once

#include "../common//Idb/Idb.h"
#include "../common/Utils/Utils.h"
#include "../common/Utils/GlobalConstants.h"
#include "MySQL-C-Api-6.1/include/mysql.h"

using namespace std;

const int MAXCONN = 2;

class MysqlDb : public Idb {

private:
	MYSQL* GetConnection() {
		//srand((unsigned int)(time(NULL)));
		size_t index = (rand() % maxConn) + 1;
		if (index > pool.size()) {
			MYSQL* pmysql;
			pmysql = mysql_init((MYSQL*)NULL);
			if (pmysql != NULL)
			{
				if (mysql_real_connect(pmysql, dbhost.c_str(), dbuser.c_str(), dbpwd.c_str(), dbname.c_str(), dbport, NULL, 0))
				{
					pool.push_back(pmysql);
					return pmysql;
				}
			}
			return nullptr;
		}
		else {
			return pool.at(index - 1);
		}
	}

public:

	MysqlDb(string dbhost, string dbuser, string dbpwd, string dbname) :
		dbhost(dbhost), dbuser(dbuser), dbpwd(dbpwd), dbname(dbname), dbport(3306), maxConn(MAXCONN)
	{

	}

	MysqlDb(string dbhost, string dbuser, string dbpwd, string dbname, int dbport, int maxConn) :
		dbhost(dbhost), dbuser(dbuser), dbpwd(dbpwd), dbname(dbname), dbport(dbport), maxConn(maxConn)
	{

	}

	MysqlDb(string dbhost, string dbuser, string dbpwd, string dbname, int dbport) :
		dbhost(dbhost), dbuser(dbuser), dbpwd(dbpwd), dbname(dbname), dbport(dbport), maxConn(MAXCONN)
	{

	}

	Rjson select(string tablename, Rjson& params, vector<string> fields = vector<string>(), int queryType = 1) override
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

	Rjson ExecQuerySql(string aQuery, vector<string> fields) {
		Rjson rs = Utils::MakeJsonObjectForFuncReturn(STSUCCESS);
		string u8Query = Utils::UnicodeToU8(aQuery);
		MYSQL* mysql = GetConnection();
		if(mysql == nullptr)
			return Utils::MakeJsonObjectForFuncReturn(STDBCONNECTERR);
		if (mysql_query(mysql, u8Query.c_str()))
		{
			string errmsg = "err at line: ";
			errmsg.append(Utils::IntTransToString(__LINE__)).append(". ");
			errmsg.append(mysql_error(mysql)).append(". error code: ");
			errmsg.append(Utils::IntTransToString(mysql_errno(mysql)));
			return rs.ExtendObject(Utils::MakeJsonObjectForFuncReturn(STDBOPERATEERR, errmsg));
		}
		else
		{
			MYSQL_RES* result = mysql_use_result(mysql);
			if (result != NULL)
			{
				MYSQL_ROW row;
				int num_fields = mysql_num_fields(result);
				MYSQL_FIELD* fields = mysql_fetch_fields(result);
				vector<Rjson> arr;
				while ((row = mysql_fetch_row(result)) && row != NULL)
				{
					Rjson al;
					for (int i = 0; i < num_fields; ++i)
					{
						al.AddValueString(fields[i].name, Utils::U8ToUnicode(row[i]));
					}
					arr.push_back(al);
				}
				if (arr.empty())
					rs.ExtendObject(Utils::MakeJsonObjectForFuncReturn(STQUERYEMPTY));
				rs.AddValueObjectArray("data", arr);
			}
			mysql_free_result(result);
		}
		cout << "SQL: " << aQuery << endl;
		return rs;
	}


	Rjson create(string tablename, Rjson& params) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}


	Rjson update(string tablename, Rjson& params) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}


	Rjson remove(string tablename, Rjson& params) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}


	Rjson querySql(string sql, Rjson& params = Rjson(), vector<string> filelds = vector<string>()) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}


	Rjson execSql(string sql) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}


	Rjson insertBatch(string tablename, vector<Rjson> elements) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}


	Rjson transGo(vector<string> sqls, bool isAsync = false) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	~MysqlDb()
	{
		while (pool.size())
		{
			mysql_close(pool.back());
			pool.pop_back();
		}
	}

private:
	vector<MYSQL*> pool;
	int maxConn;
	string dbhost;
	string dbuser;
	string dbpwd;
	string dbname;
	int dbport;
};