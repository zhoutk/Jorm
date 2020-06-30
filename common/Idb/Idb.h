#pragma once
#include "../Rjson/Rjson.h"
#include <vector>
#include <iostream>

using namespace std;

class Idb
{
public:
	virtual Rjson select(string tablename, Rjson& params, vector<string> fields = vector<string>(), int queryType = 1) = 0;
	virtual Rjson create(string tablename, Rjson& params) = 0;
	virtual Rjson update(string tablename, Rjson& params) = 0;
	virtual Rjson remove(string tablename, Rjson& params) = 0;
	virtual Rjson querySql(string sql, Rjson& params = Rjson(), vector<string> filelds = vector<string>()) = 0;
	virtual Rjson execSql(string sql) = 0;
	virtual Rjson insertBatch(string tablename, vector<Rjson> elements, string constraint = "id") = 0;
	virtual Rjson transGo(vector<string> sqls, bool isAsync = false) = 0;
};

