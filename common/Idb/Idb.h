#pragma once
#include "../Rjson/Rjson.h"
#include <vector>
#include <iostream>

using namespace std;

class Idb
{
public:
	virtual Rjson select(string tablename, Rjson& params, vector<string> fields = vector<string>()) = 0;
	virtual Rjson create(string tablename, Rjson& params) = 0;
	virtual Rjson update(string tablename, Rjson& params) = 0;
	virtual Rjson remove(string tablename, Rjson& params) = 0;
};

