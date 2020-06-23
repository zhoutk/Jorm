#pragma once

#include "../common//Idb/Idb.h"
#include "../common/Utils/Utils.h"
#include "../common/Utils/GlobalConstants.h"
#include "MySQL-C-Api-6.1/include/mysql.h"
#include "list"

using namespace std;

class MysqlDb : public Idb {

private:
	MYSQL* GetConnection() {
		int index = rand() % maxConn;
		if (pool.at(index) == NULL) {
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
		}
		return NULL;
	}

public:
	MysqlDb(string connStr) :connStr(connStr), maxConn(1) {

	}

	MysqlDb(string connStr, int maxConn) :connStr(connStr), maxConn(maxConn) {

	}

	MysqlDb(string dbhost, string dbuser, string dbpwd, string dbname) :
		dbhost(dbhost), dbuser(dbuser), dbpwd(dbpwd), dbname(dbname), dbport(3306)
	{

	}

	MysqlDb(string dbhost, string dbuser, string dbpwd, string dbname, int dbport) : 
		dbhost(dbhost), dbuser(dbuser), dbpwd(dbpwd), dbname(dbname), dbport(dbport)
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