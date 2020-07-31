#pragma once

#include "../thirds/rapidjson/document.h"
#include "../thirds/rapidjson/stringbuffer.h"
#include "../thirds/rapidjson/writer.h"
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;
using namespace rapidjson;

class Rjson {
private:
	Document* json;

public:
	Rjson() {
		json = new Document();
		json->SetObject();
	}

	Rjson(const char* jstr) {
		json = new Document();
		json->Parse(jstr);
	}

	Rjson(string jstr) {
		new (this)Rjson(jstr.c_str());
	}

	Rjson(const Rjson& origin) {
		json = new Document();
		json->CopyFrom(*(origin.json), json->GetAllocator());
	}

	string operator[](string key) {
		string rs = "";
		if (json->HasMember(key.c_str())) {
			int vType;
			GetValueAndTypeByKey(key.c_str(), &rs, &vType);
		}
		return rs;
	}

	Rjson& operator = (const Rjson& origin) {
		new (this)Rjson(origin);
		return(*this);
	}

	bool HasMember(string key) {
		return json->HasMember(key.c_str());
	}

	string GetStringValueAndRemove(string key) {
		string rs = (*this)[key];
		if (HasMember(key)) {
			json->RemoveMember(key.c_str());
		}
		return rs;
	}

	vector<Rjson> GetArrayByKey(string k) {
		vector<Rjson> rs;
		if (json->HasMember(k.c_str()) && (*json)[k.c_str()].IsArray()) {
			Value& v = (*json)[k.c_str()];
			size_t len = v.Size();
			for (size_t i = 0; i < len; i++) {
				Rjson al;
				for (auto iter = v[i].MemberBegin(); iter != v[i].MemberEnd(); ++iter)
				{
					Value vv;
					string* newK = new string(iter->name.GetString());
					vv.CopyFrom(iter->value, al.json->GetAllocator());
					al.json->AddMember(StringRef(newK->c_str()), vv, al.json->GetAllocator());
				}
				rs.push_back(al);
			}
		}
		return rs;
	}

	Rjson ExtendObject(Rjson& obj) {
		Document* src = obj.GetOriginRapidJson();
		for (auto iter = src->MemberBegin(); iter != src->MemberEnd(); ++iter)
		{
			if (json->HasMember(iter->name)) {
				Value& v = (*json)[iter->name];
				v.CopyFrom(iter->value, json->GetAllocator());
				//v = (Value&)std::move(vTmp);
			}
			else {
				string* newK = new string(iter->name.GetString());
				Value vTmp;
				vTmp.CopyFrom(iter->value, json->GetAllocator());
				json->AddMember(StringRef(newK->c_str()), vTmp, json->GetAllocator());
			}
		}
		return *(this);
	}

	void AddValueInt(string k, int v) {
		string* newK = new string(k);
		Value aInt(kNumberType);
		aInt.SetInt(v);
		
		if (json->HasMember(k.c_str())) {
			Value& v = (*json)[k.c_str()];
			v.CopyFrom(aInt, json->GetAllocator());
		}
		else {
			json->AddMember(StringRef(newK->c_str()), aInt, json->GetAllocator());
		}
	}

	void AddValueFloat(string k, double v) {
		string* newK = new string(k);
		Value aDouble(kNumberType);
		aDouble.SetDouble(v);

		if (json->HasMember(k.c_str())) {
			Value& v = (*json)[k.c_str()];
			v.CopyFrom(aDouble, json->GetAllocator());
		}
		else {
			json->AddMember(StringRef(newK->c_str()), aDouble, json->GetAllocator());
		}
	}

	void AddValueString(string k, string v) {
		string* newK = new string(k);
		Value aStr(kStringType);
		aStr.SetString(v.c_str(), json->GetAllocator());

		if (json->HasMember(k.c_str())) {
			Value& v = (*json)[k.c_str()];
			v.CopyFrom(aStr, json->GetAllocator());
		}
		else {
			json->AddMember(StringRef(newK->c_str()), aStr, json->GetAllocator());
		}
	}

	void AddValueObject(string k, Rjson v) {
		string* newK = new string(k);
		Value aObj(kObjectType);

		Document* al = v.GetOriginRapidJson();
		for (auto iter = al->MemberBegin(); iter != al->MemberEnd(); ++iter)
		{	//必须新建，要把内存放到这个json对象上，不然，目标值长度一大，就会出问题。
			string* nkey = new string(iter->name.GetString());
			Value nv;
			nv.CopyFrom(iter->value, json->GetAllocator());
			aObj.AddMember(StringRef(nkey->c_str()), nv, json->GetAllocator());
		}

		if (json->HasMember(k.c_str())) {
			Value& v = (*json)[k.c_str()];
			v.CopyFrom(aObj, json->GetAllocator());
		}
		else {
			json->AddMember(StringRef(newK->c_str()), aObj, json->GetAllocator());
		}
	}

	void AddValueArray(string k, vector<string>& arr) {
		string* newK = new string(k);
		int len = arr.size();
		Value rows(kArrayType);
		for (int i = 0; i < len; i++) {
			Value al(kStringType);
			al.SetString(arr.at(i).c_str(),json->GetAllocator());
			rows.PushBack(al, json->GetAllocator());
		}
		
		if (json->HasMember(k.c_str())) {
			Value& v = (*json)[k.c_str()];
			v.CopyFrom(rows, json->GetAllocator());
		}
		else {
			json->AddMember(StringRef(newK->c_str()), rows, json->GetAllocator());
		}
	}

	void AddValueObjectArray(string k, vector<Rjson>& arr) {
		string* newK = new string(k);
		int len = arr.size();
		Value rows(kArrayType);
		for (int i = 0; i < len; i++) {
			Value arow(kObjectType);
			Document* al = arr.at(i).GetOriginRapidJson();
			for (auto iter = al->MemberBegin(); iter != al->MemberEnd(); ++iter)
			{	//必须新建，要把内存放到这个json对象上，不然，目标值长度一大，就会出问题。
				string* nkey = new string(iter->name.GetString());
				Value nv;
				nv.CopyFrom(iter->value, json->GetAllocator());
				arow.AddMember(StringRef(nkey->c_str()), nv, json->GetAllocator());
			}
			rows.PushBack(arow, json->GetAllocator());
		}
		
		if (json->HasMember(k.c_str())) {
			Value& v = (*json)[k.c_str()];
			v.CopyFrom(rows, json->GetAllocator());
		}
		else {
			json->AddMember(StringRef(newK->c_str()), rows, json->GetAllocator());
		}
	}

	void GetValueAndTypeByKey(string key, string* v, int* vType) {
		Value::ConstMemberIterator iter = json->FindMember(key.c_str());
		if (iter != json->MemberEnd()) {
			*vType = (int)(iter->value.GetType());
			if (iter->value.IsInt()) {
				std::stringstream s;
				s << iter->value.GetInt();
				*v = s.str();
			}
			else if (iter->value.IsString()) {
				*v = iter->value.GetString();
			}
			/*else if (iter->value.IsArray()) {
				*v = GetJsonString((Value&)iter->value);
			}
			else if (iter->value.IsObject()) {
				*v = GetJsonString((Value&)iter->value);
			}
			else if (iter->value.IsDouble()) {
				*v = GetJsonString((Value&)iter->value);
			}*/
			else {
				*v = GetJsonString((Value&)iter->value);
			}
		}
		else {
			*vType = kStringType;
			*v = "";
		}
	}

	//vector<string> GetStringArray() {
	//	vector<string> rs;
	//	for (auto iter = json->Begin(); iter != json->End(); ++iter)
	//	{
	//		rs.push_back(iter->GetString());
	//	}
	//	return rs;
	//}

	vector<string> GetAllKeys() {
		vector<string> keys;
		for (auto iter = json->MemberBegin(); iter != json->MemberEnd(); ++iter)
		{
			keys.push_back((iter->name).GetString());
		}
		return keys;
	}

	bool IsObject() {
		return json->IsObject();
	}

	string GetJsonString() {
		StringBuffer strBuffer;
		Writer<StringBuffer> writer(strBuffer);
		json->Accept(writer);
		return strBuffer.GetString();
	}

	~Rjson() {
		if (json)
			delete json;
	}

private:
	Document* GetOriginRapidJson() {
		return json;
	}

	string GetJsonString(Value& v) {
		StringBuffer strBuffer;
		Writer<StringBuffer> writer(strBuffer);
		v.Accept(writer);
		return strBuffer.GetString();
	}

};
