// Jorm.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Idb.h"
#include "DbBase.h"
#include "Rjson.h"
#include "Qjson.h"
#include "ctime"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	string jsonStr = "{\"id\":123, \"username\":\"zhoutk\", \"dd\":{\"subitem\":\"a item\"}}";
	Qjson obj(jsonStr);
	Qjson o(obj);
	cout << o["id"] << o["username"] << o["dd"] ;

	//DbBase* db = new DbBase("D:\\codes\\Jorm\\Sqlit3\\db.db");
	//DbBase* db = new DbBase("localhost", "root", "123456", "jorm");
	//DbBase* db = new DbBase("dbname = Jorm user = root password = 123456 \
      hostaddr = 127.0.0.1 port = 5432", "Postgres");

	//DbBase* db = new DbBase("HR", "a1b2c3", "127.0.0.1:1521/xe");

	/*Rjson obj;
	obj.AddValueString("username", "插入测试");
	obj.AddValueInt("password", 3245);
	Rjson rs = db->create("users", obj);
	cout << "The result is : " << rs.GetJsonString() << endl;*/

	//vector<string> sqls;
	//sqls.push_back("insert into users (password, username) values ('4312', 'tranGo 测试') ");
	//sqls.push_back("update users set password = 9876 where id = 14");
	//Rjson rs = db->transGo(sqls);
	//cout << "The result is : " << rs.GetJsonString() << endl;

	//char* strTmp = "abcdefghijklmnopqrstuvwxyz";
	//clock_t start = clock();
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
	//	db->create("users", qObj);
	//}
	//clock_t end = clock();
	//cout << "花费了" << (double)(end - start) / CLOCKS_PER_SEC << "秒" << endl;

	//vector<Rjson> els;
	//char* strTmp = "abcdefghijklmnopqrstuvwxyz";
	//clock_t start = clock();
	//for (int i = 0; i < 3; i++) {
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
	//	qObj.AddValueString("update_time", Utils::GetLocalNowTime());
	//	els.push_back(qObj);
	//}

	//Rjson rs = db->insertBatch("users", els);			//, Utils::MakeVectorInitFromString("id,username,password")
	//cout << "The result is : " << rs.GetJsonString() << endl;
	//clock_t end = clock();
	//cout << "花费了" << (double)(end - start) / CLOCKS_PER_SEC << "秒" << endl;



	//Rjson obj;
	///*obj.AddValueString("username", "大张伟");
	//obj.AddValueString("password", "3252");*/
	////Rjson rs = db->create("users", obj);
	//Rjson rs = db->select("users", obj);

	//vector<string> keys = rs.GetAllKeys();

	//size_t len = keys.size();
	//for (size_t i = 0; i < len; i++) {
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




	//obj.AddValueString("username", "张三");
	//rs = db->select("users", obj);
	//cout << "22222222 the result is : " << rs.GetJsonString() << endl;

	//rs = db->select("users", obj);
	//cout << "22222222 the result is : " << rs.GetJsonString() << endl;

	system("pause");
	return 0;

}

