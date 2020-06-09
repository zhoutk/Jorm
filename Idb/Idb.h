#pragma once
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <vector>
#include <iostream>

using namespace std;
using namespace rapidjson;

	class Idb
	{
	public:
		virtual Document retrieve(string tablename, Document* params, vector<string> fields = vector<string>()) = 0;
	};

