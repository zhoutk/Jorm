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
	MysqlDb(string connStr) :connStr(connStr), maxConn(MAXCONN) {

	}

	MysqlDb(string connStr, int maxConn) :connStr(connStr), maxConn(maxConn) {

	}

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

			if (queryType == 1) {
				querySql.append("select ").append(fieldsJoinStr).append(" from ").append(tablename);
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
	string connStr;
	int maxConn;
	string dbhost;
	string dbuser;
	string dbpwd;
	string dbname;
	int dbport;
};