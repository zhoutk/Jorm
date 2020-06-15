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

	Rjson& operator = (const Rjson& origin) {
		new (this)Rjson(origin);
		return(*this);
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
					string *newK = new string(iter->name.GetString());
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
		json->AddMember(StringRef(newK->c_str()), aInt, json->GetAllocator());
	}

	void AddValueString(string k, string v) {
		string* newK = new string(k);
		Value aStr(kStringType);
		aStr.SetString(v.c_str(), json->GetAllocator());
		json->AddMember(StringRef(newK->c_str()), aStr, json->GetAllocator());
	}

	void AddValueObjectArray(string k, vector<Rjson> arr) { //些处arr是值传递。
		string* newK = new string(k);
		int len = arr.size();
		Value rows(kArrayType);
		for (int i = 0; i < len; i++) {
			Value arow(kObjectType);
			Document* al = arr.at(i).GetOriginRapidJson();
			for (auto iter = al->MemberBegin(); iter != al->MemberEnd(); ++iter)
			{	//arr中的数据会被move走，些后arr不能再引用
				arow.AddMember(iter->name, iter->value, json->GetAllocator()); 
			}
			rows.PushBack(arow, json->GetAllocator());
		}
		json->AddMember(StringRef(newK->c_str()), rows, json->GetAllocator());
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
			else {
				*v = "";
			}
		}
		else {
			*vType = kStringType;
			*v = "";
		}
	}

	Document* GetOriginRapidJson() {
		return json;
	}

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

};
