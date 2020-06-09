#pragma once
#include "Idb.h"
#include "Sqlit3Db.h"

class DbBase
{
public:
	DbBase(string connStr) : connStr(connStr) {
		db = new Sqlit3Db(connStr);
	};
	~DbBase();

	Document retrieve(string tablename, Document* params, vector<string> fields = vector<string>());

private:
	string connStr;
	Idb * db;
};

