#pragma once

#include "../common/Idb/Idb.h"
#include "../common/Utils/Utils.h"
#include "../common/Utils/GlobalConstants.h"

using namespace std;

namespace Oracle {

	const int MAXCONN = 2;
	char* QUERY_EXTRA_KEYS[] = { "ins", "lks", "ors" };
	char* QUERY_UNEQ_OPERS[] = { ">,", ">=,", "<,", "<=,", "<>,", "=," };

	class OracleDb : public Idb 
	{

	public:
		Rjson select(string tablename, Rjson& params, vector<string> fields = vector<string>(), int queryType = 1) override
		{
			throw std::logic_error("The method or operation is not implemented.");
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

	};
}