#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QDebug>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

class Qjson {
private:
	QJsonObject* json;

public:
	Qjson() {
		json = new QJsonObject();
	}

	Qjson(const char* jstr) {
		QJsonDocument jsonDocument = QJsonDocument::fromJson(jstr);
		json = new QJsonObject(jsonDocument.object());
	}

	Qjson(string jstr) {
		new (this)Qjson(jstr.c_str());
	}

	Qjson(QString jstr) {
		new (this)Qjson(jstr.toStdString());
	}

	Qjson(const Qjson& origin) {
		new (this)Qjson(QString(QJsonDocument(*(origin.json)).toJson(QJsonDocument::Compact)));
	}

	string operator[](string key) {
		string rs = "";
		if (json->contains(key.c_str())) {
			int vType;
			GetValueAndTypeByKey(key.c_str(), &rs, &vType);
		}
		return rs;
	}

	QString GetJsonQString() {
		return QString(QJsonDocument(*json).toJson());
	}

	string GetJsonString() {
		return QString(QJsonDocument(*json).toJson(QJsonDocument::Compact)).toStdString();
	}

	void GetValueAndTypeByKey(string key, string* v, int* vType) {
		QJsonObject::iterator iter = json->find(key.c_str());
		if (iter != json->end()) {
			*vType = (int)(iter->type());
			if (iter->isNull()) {
				*v = "null";
			}
			else if (iter->isBool()) {
				*v = QString("%1").arg(iter->toBool()).toStdString();
			}
			else if (iter->isDouble()) {
				*v = QString("%1").arg(iter->toDouble()).toStdString();
			}
			else if (iter->isString()) {
				*v = iter->toString().toStdString();
			}
			else if (iter->isArray()) {
				*v = QJsonDocument(iter->toArray()).toJson(QJsonDocument::Compact);
			}
			else if (iter->isObject()) {
				*v = QJsonDocument(iter->toObject()).toJson(QJsonDocument::Compact);
			}
			else if (iter->isUndefined()) {
				*v = "undefined";
			}
			else {
				*v = "";
			}
		}
		else {
			*vType = QJsonValue::String;
			*v = "";
		}
	}

	~Qjson() {
		if (json)
			delete json;
	}

private:
	QString GetJsonQString(QJsonObject& v) {
		return QString(QJsonDocument(v).toJson());
	}

	string GetJsonString(QJsonObject& v) {
		return QString(QJsonDocument(v).toJson()).toStdString();
	}
};