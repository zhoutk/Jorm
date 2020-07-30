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

	string GetJsonString() {
		return QString(QJsonDocument(*json).toJson()).toStdString();
	}

	~Qjson() {
		if (json)
			delete json;
	}

};