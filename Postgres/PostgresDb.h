#pragma once

#include "../common/Idb/Idb.h"
#include "../common/Utils/Utils.h"
#include "../common/Utils/GlobalConstants.h"
#include "../thirds/postgres/pqxx/pqxx"

using namespace std;

namespace Postgres {

	class PostgresDb : public Idb
	{
	public:
		Rjson select(string tablename, Rjson& params, vector<string> fields = vector<string>(), int queryType = 1) override
		{
			return Utils::MakeJsonObjectForFuncReturn(STSUCCESS);
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

	};

}