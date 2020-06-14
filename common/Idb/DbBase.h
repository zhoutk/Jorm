#pragma once
#include "Idb.h"
#include "../Sqlit3/Sqlit3Db.h"

class DbBase
{
public:
	DbBase(string connStr) : connStr(connStr) {
		db = new Sqlit3Db(connStr);
	};
	~DbBase() {
		if (db) {
			delete db;
		}
	};

	Rjson retrieve(string tablename, Rjson& params, vector<string> fields = vector<string>()) {
		return db->retrieve(tablename, params, fields);
	};

private:
	string connStr;
	Idb * db;
};

