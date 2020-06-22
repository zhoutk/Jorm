// Jorm.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Idb.h"
#include "DbBase.h"
#include "Rjson.h"
#include "ctime"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	DbBase * db = new DbBase("D:\\codes\\Jorm\\Sqlit3\\db.db");

	char* strTmp = "abcdefghijklmnopqrstuvwxyz";
	clock_t start = clock();
	for (int i = 0; i < 100; i++) {
		Rjson qObj;
		qObj.AddValueInt("password", rand());
		int len = rand() % 5 + 5;
		string uname = "";
		for (int j = 0; j < len; j++) {
			int dd = rand() % 26;
			char ddd = strTmp[rand() % 26];
			uname.append(1, strTmp[rand() % 26]);
		}
		qObj.AddValueString("username", uname);
		db->create("users", qObj);
	}
	clock_t end = clock();
	cout << "花费了" << (double)(end - start) / CLOCKS_PER_SEC << "秒" << endl;

	//vector<Rjson> els;
	//char* strTmp = "abcdefghijklmnopqrstuvwxyz";
	//for (int i = 0; i < 100; i++) {
	//	Rjson qObj;
	//	qObj.AddValueInt("password", rand());
	//	int len = rand() % 5 + 5;
	//	string uname = "";
	//	for (int j = 0; j < len; j++) {
	//		int dd = rand() % 26;
	//		char ddd = strTmp[rand() % 26];
	//		uname.append(1, strTmp[rand() % 26]);
	//	}
	//	qObj.AddValueString("username", uname);
	//	els.push_back(qObj);
	//}

	//clock_t start = clock();
	//Rjson rs = db->insertBatch("users", els);			//, Utils::MakeVectorInitFromString("id,username,password")
	//clock_t end = clock();
	//cout << "花费了" << (double)(end - start) / CLOCKS_PER_SEC << "秒" << endl;


	//vector<string> keys = rs.GetAllKeys();

	//int len = keys.size();
	//for (int i = 0; i < len; i++) {
	//	string v, key = keys[i];
	//	int vType;
	//	rs.GetValueAndTypeByKey(key, &v, &vType);
	//	if (vType == 4) {
	//		cout << "    " << key << " is array: " << endl;
	//		cout << "-------------------------------------" << endl;
	//		vector<Rjson> arr = rs.GetArrayByKey(key);
	//		for (size_t j = 0; j < arr.size(); j++) {
	//			Rjson al = arr[j];
	//			vector<string> subKeys = al.GetAllKeys();
	//			for (size_t k = 0; k < subKeys.size(); k++) {
	//				string sv;
	//				int nstype;
	//				al.GetValueAndTypeByKey(subKeys[k], &sv, &nstype);
	//				cout << "    " << subKeys[k] << ", value: " << sv << endl;
	//			}
	//			cout << endl;
	//		}
	//		//cout << "-------------------------------------" << endl;
	//	}else
	//		cout << "key: " << key << ", value: " << v << endl;
	//}

	//cout << "22222222 the result is : " << rs.GetJsonString() << endl;

	system("pause");
	return 0;

}

