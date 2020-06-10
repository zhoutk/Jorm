#include "DbBase.h"

DbBase::~DbBase()
{
	if (db) {
		delete db;
	}
}

Document DbBase::retrieve(string tablename, Document& params, vector<string> fields)
{
	return db->retrieve(tablename, params, fields);
}

