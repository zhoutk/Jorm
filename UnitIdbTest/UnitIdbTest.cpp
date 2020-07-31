#include "pch.h"
#include "CppUnitTest.h"
#include "../common/Idb/DbBase.h"
#include "../common/Rjson/Rjson.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitIdbTest
{
	DbBase* db = new DbBase("D:\\codes\\Jorm\\Sqlit3\\db.db");
	//DbBase* db = new DbBase("localhost", "root", "123456", "jorm");
	//DbBase* db = new DbBase("dbname = Jorm user = root password = 123456 \
      hostaddr = 127.0.0.1 port = 5432", "Postgres");

	TEST_CLASS(UnitIdbTest)
	{
	public:
		TEST_METHOD(TestDbQuerySelectStar)							//test select * from ...
		{
			Rjson qObj;
			Rjson rs1 = db->select("users", qObj);
			string v1;
			int vType;
			rs1.GetValueAndTypeByKey("code", &v1, &vType);
			Assert::AreEqual(atoi(v1.c_str()), (int)STSUCCESS);
		}
		TEST_METHOD(TestDbQuerySelectFields)						//test select id,..... from ...
		{
			Rjson qObj;
			string str[] = { "id","password", "username" };
			vector<string> fields(str, str + sizeof(str) / sizeof(str[0]));
			Rjson rs = db->select("users", qObj, fields);

			string v;
			int vType;
			rs.GetValueAndTypeByKey("code", &v, &vType);
			Assert::AreEqual(atoi(v.c_str()), (int)STSUCCESS);
		}
		TEST_METHOD(TestDbQuerySelectEqualResultExistAndUpdate)						//test select filed condition equal & result exist & value is chinese
		{
			Rjson qObj("{\"username\":\"张三\"}");
			Rjson rs = db->select("users", qObj);

			string v;
			int vType;
			rs.GetValueAndTypeByKey("code", &v, &vType);
			Assert::AreEqual(atoi(v.c_str()), (int)STSUCCESS);
			//size_t dd = (rs.GetArrayByKey("data")).size();
			Assert::IsTrue(rs.GetArrayByKey("data").size() > 0);

			vector<Rjson> rsData = rs.GetArrayByKey("data");					//get id
			Rjson data0 = rsData[0];
			string id = "";
			data0.GetValueAndTypeByKey("id", &id, &vType);

			string sql = "update users set update_time = '" + Utils::GetLocalNowTime() + "' where id = " + id;
			//Rjson qUp("{\"username\":\"张三\"}");								//set params for update
			//qUp.AddValueInt("id", atoi(id.c_str()));
			//qUp.AddValueString("password", "123321");
			//qUp.AddValueString("update_time", Utils::GetLocalNowTime());
			rs = db->execSql(sql);

			v = "";
			rs.GetValueAndTypeByKey("code", &v, &vType);
			Assert::AreEqual(atoi(v.c_str()), (int)STSUCCESS);				//assert update with id
		}

		TEST_METHOD(TestDbQueryManualSelectStar)						//test manual sql, not fileds
		{
			Rjson rs = db->querySql("select * from users where id = 1");

			string v;
			int vType;
			rs.GetValueAndTypeByKey("code", &v, &vType);
			Assert::AreEqual(atoi(v.c_str()), (int)STSUCCESS);
			//size_t dd = (rs.GetArrayByKey("data")).size();
			Assert::IsTrue(rs.GetArrayByKey("data").size() > 0);
		}

		TEST_METHOD(TestDbQueryManualSelectFields)						//test manual sql, with fileds
		{
			Rjson obj;
			obj.AddValueString("username", "张三");
			string str[] = { "id","password", "username" };
			vector<string> fields(str, str + sizeof(str) / sizeof(str[0]));
			Rjson rs = db->querySql("select * from users where id = 1", obj, fields);

			string v;
			int vType;
			rs.GetValueAndTypeByKey("code", &v, &vType);
			Assert::AreEqual(atoi(v.c_str()), (int)STSUCCESS);
			//size_t dd = (rs.GetArrayByKey("data")).size();
			Assert::IsTrue(rs.GetArrayByKey("data").size() > 0);
		}
		TEST_METHOD(TestDbQuerySelectEqualResultNotExist)						//test select filed condition equal & result not exist
		{
			Rjson qObj("{\"username\":\"张三丰不存在\"}");
			Rjson rs = db->select("users", qObj);

			string v;
			int vType;
			rs.GetValueAndTypeByKey("code", &v, &vType);
			Assert::AreEqual(atoi(v.c_str()), (int)STQUERYEMPTY);
			//size_t dd = (rs.GetArrayByKey("data")).size();
			Assert::IsTrue(rs.GetArrayByKey("data").size() == 0);
		}
		TEST_METHOD(TestDbQueryParamsError)						//test params isObject is false
		{
			Rjson qObj("{");
			Rjson rs = db->select("users", qObj);

			string v;
			int vType;
			rs.GetValueAndTypeByKey("code", &v, &vType);
			Assert::AreEqual(atoi(v.c_str()), (int)STPARAMERR);
		}
		TEST_METHOD(TestDbQueryFiledError)						//test select filed is not exist
		{
			Rjson qObj("{\"username1\":\"张三\"}");
			Rjson rs = db->select("users", qObj);

			string v;
			int vType;
			rs.GetValueAndTypeByKey("code", &v, &vType);
			Assert::AreEqual(atoi(v.c_str()), (int)STDBOPERATEERR);
		}
		TEST_METHOD(TestDbCURDRecord)						//test curd
		{
			Rjson qObj("{\"username\":\"麻子测试\", \"password\":1298}");
			Rjson rs = db->create("users", qObj);

			string v;
			int vType;
			rs.GetValueAndTypeByKey("code", &v, &vType);
			Assert::AreEqual(atoi(v.c_str()), (int)STSUCCESS);				//assert create

			v = "";
			rs = db->select("users", Rjson("{\"username\":\"麻子测试\"}"));
			rs.GetValueAndTypeByKey("code", &v, &vType);
			Assert::AreEqual(atoi(v.c_str()), (int)STSUCCESS);				//assert query =

			vector<Rjson> rsData = rs.GetArrayByKey("data");
			Rjson data0 = rsData[0];
			string id = "";
			data0.GetValueAndTypeByKey("id", &id, &vType);

			Rjson qUp("{\"username\":\"麻子修改\"}");
			rs = db->update("users", qUp);

			v = "";
			rs.GetValueAndTypeByKey("code", &v, &vType);
			Assert::AreEqual(atoi(v.c_str()), (int)STPARAMERR);				//assert update no id

			qUp.AddValueInt("id", atoi(id.c_str()));
			qUp.AddValueString("password", "123321");
			rs = db->update("users", qUp);

			v = "";
			rs.GetValueAndTypeByKey("code", &v, &vType);
			Assert::AreEqual(atoi(v.c_str()), (int)STSUCCESS);				//assert update with id



			string sss = "{\"id\":" + id + "}";
			Rjson rData(sss);
			//rData.AddValueInt("id", atoi(v.c_str()));
			rs = db->remove("users", rData);
			v = "";
			rs.GetValueAndTypeByKey("code", &v, &vType);
			Assert::AreEqual(atoi(v.c_str()), (int)STSUCCESS);				//assert remove

		}
	};
}
