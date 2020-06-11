#pragma once

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <vector>
#include <sstream>

using namespace std;
using namespace rapidjson;

class Rjson{
private:
	Document* json;

public:
	Rjson() {
		json = new Document();
		json->SetObject();
	}

	Rjson(char* jstr) {
		json = new Document();
		json->Parse(jstr);
	}

	Rjson(string jstr) {
		new (this)Rjson(jstr.c_str());
	}

	Rjson(const Rjson& origin) {
		StringBuffer strBuffer;
		Writer<StringBuffer> writer(strBuffer);
		(origin.json)->Accept(writer);
		Document* cp = new Document();
		cp->Parse(strBuffer.GetString());
		json = cp;
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

	void AddValueObjectArray(string k, vector<Rjson> arr) {
		string* newK = new string(k);
		int len = arr.size();
		Value rows(kArrayType);
		for (int i = 0; i < len; i++) {
			Value arow(kObjectType);
			Document* al = arr.at(i).GetOriginRapidJson();
			for (auto iter = al->MemberBegin(); iter != al->MemberEnd(); ++iter)
			{
				arow.AddMember(iter->name, iter->value, json->GetAllocator());
			}
			rows.PushBack(arow, json->GetAllocator());
		}
		json->AddMember(StringRef(newK->c_str()), rows, json->GetAllocator());
	}

	void GetValueAndTypeByKey(string key, string* v, bool* v_number) {
		Value& value = json->FindMember(key.c_str())->value;
		if (value.IsInt() || value.IsInt64() || value.IsFloat() || value.IsDouble()) {
			*v_number = true;
			std::stringstream s;
			s << value.GetInt();
			*v = s.str();
		}
		else {
			*v_number = false;
			*v = value.GetString();
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
