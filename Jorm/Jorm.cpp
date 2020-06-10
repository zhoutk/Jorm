// Jorm.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Idb.h"
#include "DbBase.h"
#include <iostream>
#include "rapidjson/document.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	DbBase * db = new DbBase("D:\\codes\\Jorm\\Sqlit3\\db.db");
	Document qObj;
	qObj.Parse("{\"username\", \"john\"}");
	vector<string> fields;
	fields.push_back("username");
	fields.push_back("password");
	Document rs = db->retrieve("users", &qObj, fields);

	StringBuffer strBuffer;
	Writer<StringBuffer> writer(strBuffer);
	rs.Accept(writer);

	cout << "the result is : " << strBuffer.GetString() << endl;

	system("pause");
	return 0;
}

