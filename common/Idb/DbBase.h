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

	Rjson select(string tablename, Rjson& params, vector<string> fields = vector<string>(), int queryType = 1) {
		return db->select(tablename, params, fields);
	};

	Rjson create(string tablename, Rjson& params) {
		return db->create(tablename, params);
	};

	Rjson update(string tablename, Rjson& params) {
		return db->update(tablename, params);
	};

	Rjson remove(string tablename, Rjson& params) {
		return db->remove(tablename, params);
	};

	Rjson querySql(string sql, Rjson& params = Rjson(), vector<string> filelds = vector<string>()) {
		return db->querySql(sql, params, filelds);
	}

private:
	string connStr;
	Idb * db;
};

