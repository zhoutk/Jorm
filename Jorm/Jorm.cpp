// Jorm.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Idb.h"
#include "DbBase.h"
#include "Rjson.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	DbBase * db = new DbBase("D:\\codes\\Jorm\\Sqlit3\\db.db");
	Rjson qObj("{\"id\": 1, \"password\":\"123\", \"username\":\"张三\"}");		//\"username\": \"john\"
	string str[] = { "id","password", "username" };
	vector<string> fields(str, str + sizeof(str) / sizeof(str[0]));
	Rjson rs = db->retrieve("users", qObj, fields);			//, &qObj, fields

	cout << "the result is : " << rs.GetJsonString() << endl;

	system("pause");
	return 0;
}

