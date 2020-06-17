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

	/*Rjson qObjCreate("{\"id\":9}");
	Rjson rs = db->remove("users", qObjCreate);*/

	//Rjson qObj("{\"id\": 1, \"password\":\"123\", \"username\":\"张三\"}");		//\"username\": \"john\"
	Rjson qObj;		//("{")    \"username\": \"john\"    ("{\"username\":\"张三\"}")
	qObj.AddValueString("ors", "password,123,password,张");
	//string str[] = { "password", "username" };   //"username", "password", "update_time"
	//vector<string> fields(str, str + sizeof(str) / sizeof(str[0]));
	Rjson rs = db->select("users", qObj, Utils::MakeVectorInitForString("id,username,password,update_time"));			//, &qObj, fields

	//Rjson obj;
	//obj.AddValueString("username", "张三");
	//string str[] = { "password", "123", "321" };   //"username", "password", "update_time"
	//vector<string> fields(str, str + sizeof(str) / sizeof(str[0]));
	//obj.AddValueArray("ins", fields);

	//string dd = obj.GetStringValueAndRemove("username");

	//string cc = obj["ins"];

	//Rjson arr(cc);

	//vector<string> list = arr.GetStringArray();

	

	/*string str[] = { "id","password", "username" };
	vector<string> fields(str, str + sizeof(str) / sizeof(str[0]));
	Rjson rs = db->querySql("select * from users where id = 1", obj );*/

	//cout << "1111111 the result is : " << rs.GetJsonString() << endl;

	vector<string> keys = rs.GetAllKeys();

	int len = keys.size();
	for (int i = 0; i < len; i++) {
		string v, key = keys[i];
		int vType;
		rs.GetValueAndTypeByKey(key, &v, &vType);
		if (vType == 4) {
			cout << "    " << key << " is array: " << endl;
			cout << "-------------------------------------" << endl;
			vector<Rjson> arr = rs.GetArrayByKey(key);
			for (size_t j = 0; j < arr.size(); j++) {
				Rjson al = arr[j];
				vector<string> subKeys = al.GetAllKeys();
				for (size_t k = 0; k < subKeys.size(); k++) {
					string sv;
					int nstype;
					al.GetValueAndTypeByKey(subKeys[k], &sv, &nstype);
					cout << "    " << subKeys[k] << ", value: " << sv << endl;
				}
				cout << endl;
			}
			//cout << "-------------------------------------" << endl;
		}else
			cout << "key: " << key << ", value: " << v << endl;
	}

	//cout << "22222222 the result is : " << rs.GetJsonString() << endl;

	system("pause");
	return 0;

}

