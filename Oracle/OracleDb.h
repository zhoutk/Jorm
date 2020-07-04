#pragma once

#include "../common/Idb/Idb.h"
#include "../common/Utils/Utils.h"
#include "../common/Utils/GlobalConstants.h"

#include "../thirds/oracle/include/occi.h"
using namespace oracle::occi;

using namespace std;

namespace Oracle {

	const int MAXCONN = 2;
	char* QUERY_EXTRA_KEYS[] = { "ins", "lks", "ors" };
	char* QUERY_UNEQ_OPERS[] = { ">,", ">=,", "<,", "<=,", "<>,", "=," };

	class OracleDb : public Idb
	{

	private:
		Connection* GetConnection(string* err = nullptr) {
			//srand((unsigned int)(time(NULL)));
			try {
				size_t index = (rand() % maxConn) + 1;
				Environment* env = Environment::createEnvironment();
				if (env == nullptr) {
					return nullptr;
				}
				else {
					if (index > pool.size()) {
						Connection* orclSql;
						orclSql = env->createConnection(dbuser, dbpwd, dbtns);
						if (orclSql != NULL)
						{
							pool.push_back(orclSql);
							return orclSql;
						}
						else
							return nullptr;
					}
					else {
						return pool.at(index - 1);
					}
				}
			}
			catch (oracle::occi::SQLException& e) {
				err = new string(e.what());
				return nullptr;
			}
		}

	public:

		OracleDb(string dbuser, string dbpwd, string dbtns) : dbuser(dbuser), dbpwd(dbpwd), dbtns(dbtns), maxConn(MAXCONN) {
		}

		Rjson select(string tablename, Rjson& params, vector<string> fields = vector<string>(), int queryType = 1) override
		{
			return ExecQuerySql("select * from users", fields);
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


		Rjson insertBatch(string tablename, vector<Rjson> elements, string constraint = "id") override
		{
			throw std::logic_error("The method or operation is not implemented.");
		}


		Rjson transGo(vector<string> sqls, bool isAsync = false) override
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

	private:

		Rjson ExecQuerySql(string aQuery, vector<string> fields) {
			Rjson rs = Utils::MakeJsonObjectForFuncReturn(STSUCCESS);
			string u8Query = Utils::UnicodeToU8(aQuery);
			string err;
			Connection* conn = GetConnection(&err);
			if (conn == nullptr)
				return Utils::MakeJsonObjectForFuncReturn(STDBCONNECTERR, Utils::U8ToUnicode((char*)err.c_str()));
			try {
				Statement* stmt = conn->createStatement();
				ResultSet * R = stmt->executeQuery(u8Query);

				auto coLen = R->getColumnListMetaData();
				/*vector<Rjson> arr;
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

				cout << "SQL: " << aQuery << endl;*/
				return rs;
			}
			catch (const std::exception& e) {
				return Utils::MakeJsonObjectForFuncReturn(STDBOPERATEERR, Utils::U8ToUnicode((char*)e.what()));
			}
		}

	private:
		vector<Connection*> pool;
		int maxConn;
		string dbuser;
		string dbpwd;
		string dbtns;

	};
}