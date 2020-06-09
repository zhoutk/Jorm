#pragma once
#include "Idb.h"

class DbBase :
    public Idb
{
public:
    DbBase() : table("") {};
    DbBase(string table) : table(table) {};

    Document retrieve(string tablename, Document params, vector<string> fields = vector<string>());

private:
    string table;
};

