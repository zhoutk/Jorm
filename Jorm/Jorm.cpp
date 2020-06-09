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
	DbBase db;
	Document rs = db.retrieve("users", Document());

	StringBuffer strBuffer;
	Writer<StringBuffer> writer(strBuffer);
	rs.Accept(writer);

	cout << "the result is : " << strBuffer.GetString() << endl;

	system("pause");
	return 0;
}

