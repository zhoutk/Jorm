#include "DbBase.h"

DbBase::~DbBase()
{
	if (db) {
		delete db;
	}
}

Rjson DbBase::retrieve(string tablename, Rjson& params, vector<string> fields)
{
	return db->retrieve(tablename, params, fields);
}

