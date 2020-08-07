#pragma once
#include <windows.h>
#include "../Rjson/Rjson.h"
#include <sstream>
#include <time.h>
#include "GlobalConstants.h"
#include <algorithm>
#include <QString>

class Utils {
public:
	static bool FindStartsStringFromVector(vector<string> strs, string value) {
		bool rs = false;
		size_t len = strs.size();
		for (size_t i = 0; i < len; i++) {
			size_t vlen = value.size();
			string key = strs[i];
			size_t klen = key.size();
			if (vlen <= klen)
				continue;
			else {
				if (value.substr(0, klen).compare(key) == 0)
				{
					rs = true;
					break;
				}
			}
		}
		return rs;
	}

	static bool FindStartsCharArray(char** strings, char* value) {
		char* string;
		char* parValue;
		while ((string = *strings++) != NULL)
		{
			parValue = value;
			while (*string != '\0' || *parValue != '\0')
			{
				if (*string == *parValue++)
				{
					string++;
					continue;
				}
				else if (*string == '\0') {
					return true;
				}
				else {
					break;
				}
				parValue++;
			}
		}
		return false;
	}

	static bool FindStringFromVector(vector<string> strs, string value) {
		auto iter = std::find(strs.begin(), strs.end(), value);
		if (iter == strs.end()) {
			return false;
		}
		else {
			return true;
		}
	}

	static bool FindCharArray(char** strings, char* value) {
		char* string;     
		char* parValue;  
		while ((string = *strings++) != NULL)
		{
			parValue = value;
			while (*string != '\0' && *parValue != '\0') 
			{
				if (*string++ == *parValue++)  
				{
					if((*parValue == '\0'))
						return true;
				}
				else {
					break;
				}
			}
		}
		return false;
	}

	static vector<string> MakeVectorInitFromString(string str, char flag = ',') {
		vector<string> rs;
		istringstream iss(str);
		string temp;

		while (getline(iss, temp, flag)) {
			rs.push_back(temp);
		}
		return rs;
	}

	static string GetLocalNowTime() {
		time_t timep;
		time(&timep);
		char tmp[64];
		struct tm nowTime;
		localtime_s(&nowTime, &timep);
		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", &nowTime);
		return std::string(tmp);
	}

	static string IntTransToString(int val) {
		std::stringstream ss;
		ss << val;
		return ss.str();
	}

	static string GetVectorJoinStr(vector<string> v) {
		std::stringstream ss;
		for (size_t i = 0; i < v.size(); ++i)
		{
			if (i != 0)
				ss << ",";
			ss << v[i];
		}
		return ss.str();
	}

	static Rjson MakeJsonObjectForFuncReturn(StatusCodes code, string info = "") {
		Rjson rs;
		rs.AddValueInt("code", (int)code);
		if (!info.empty()) {
			info.insert(0, "\r\ndetails, ");
		}
		info.insert(0, STCODEMESSAGES[(int)code]);
		rs.AddValueString("msg", info);
		return rs;
	}

	static char* U8ToUnicode(char* szU8)
	{
		QString sz = QString::fromUtf8(szU8);
		QByteArray tt = sz.toLocal8Bit();
		int len = tt.size();
		char* dd = new char[len + 1];
		memset(dd, 0, len + 1);
		memcpy(dd, tt.data(), len);
		return dd;


		////UTF8 to Unicode
		////Ԥת�����õ�����ռ�Ĵ�С
		//int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
		////����ռ�Ҫ��'\0'�����ռ䣬MultiByteToWideChar�����'\0'�ռ�
		//wchar_t* wszString = new wchar_t[wcsLen + 1];
		////ת��
		//::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);
		////������'\0'
		//wszString[wcsLen] = '\0';

		//char* m_char;
		//int len = WideCharToMultiByte(CP_ACP, 0, wszString, wcslen(wszString), NULL, 0, NULL, NULL);
		//m_char = new char[len + 1];
		//WideCharToMultiByte(CP_ACP, 0, wszString, wcslen(wszString), m_char, len, NULL, NULL);
		//m_char[len] = '\0';
		//return m_char;
	}

	static char* UnicodeToU8(string str)
	{
		QString sz = QString::fromLocal8Bit(str.c_str());
		string tt = sz.toStdString();
		int len = tt.size();
		char* dd = new char[len + 1];
		memset(dd, 0, len + 1);
		memcpy(dd, tt.c_str(), len);
		return dd; 

		//if (str.size() == 0)
		//	return "";
		//wchar_t* wszString = multiByteToWideChar(str);
		//// unicode to UTF8
		////Ԥת�����õ�����ռ�Ĵ�С������õĺ��������������෴
		//int u8Len = ::WideCharToMultiByte(CP_UTF8, NULL, wszString, wcslen(wszString), NULL, 0, NULL, NULL);
		////ͬ�ϣ�����ռ�Ҫ��'\0'�����ռ�
		////UTF8��Ȼ��Unicode��ѹ����ʽ����Ҳ�Ƕ��ֽ��ַ��������Կ�����char����ʽ����
		//char* szU8 = new char[u8Len + 1];
		////ת��
		////unicode���Ӧ��strlen��wcslen
		//::WideCharToMultiByte(CP_UTF8, NULL, wszString, wcslen(wszString), szU8, u8Len, NULL, NULL);
		////������'\0'
		//szU8[u8Len] = '\0';
		//return szU8;
	}


	static wchar_t* multiByteToWideChar(const string& pKey)
	{
		//QString sz = QString::fromLocal8Bit(pKey.c_str());
		//wchar_t* pw = new wchar_t[64];
		//int tt = sz.toWCharArray(pw);
		//return pw;

		char* pCStrKey = (char *)pKey.c_str();
		//��һ�ε��÷���ת������ַ������ȣ�����ȷ��Ϊwchar_t*���ٶ����ڴ�ռ�
		int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
		wchar_t* pWCStrKey = new wchar_t[pSize];
		//�ڶ��ε��ý����ֽ��ַ���ת����˫�ֽ��ַ���
		MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
		return pWCStrKey;
	}

};

