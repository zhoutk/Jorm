#pragma once
#include "Idb.h"
#include "sqlit3/sqlite3.h"

using namespace std;

#define SQLITECPP_ASSERT(expression, message)   assert(expression && message)

const int   OPEN_READONLY = SQLITE_OPEN_READONLY;
const int   OPEN_READWRITE = SQLITE_OPEN_READWRITE;
const int   OPEN_CREATE = SQLITE_OPEN_CREATE;
const int   OPEN_URI = SQLITE_OPEN_URI;

const int   OK = SQLITE_OK;

	class Sqlit3Db : public Idb
	{
	public:
		Sqlit3Db(const char* apFilename,
			const int   aFlags = OPEN_READONLY,
			const int   aBusyTimeoutMs = 0,
			const char* apVfs = nullptr);

		Sqlit3Db(const std::string& aFilename,
			const int          aFlags = OPEN_READONLY,
			const int          aBusyTimeoutMs = 0,
			const std::string& aVfs = "") {
			Sqlit3Db(aFilename.c_str(), aFlags, aBusyTimeoutMs, aVfs.empty() ? nullptr : aVfs.c_str());
		};

		~Sqlit3Db();

		struct Deleter
		{
			void operator()(sqlite3* apSQLite);
		};

		Document retrieve(string tablename, Document *params, vector<string> fields = vector<string>());

		sqlite3* getHandle()
		{
			return mSQLitePtr.get();
		}

		void check(const int aRet)
		{
			if (OK != aRet)
			{
				throw "SQLite Error, return code: " + aRet;
			}
		}

	private:
		// TODO: perhaps switch to having Statement sharing a pointer to the Connexion
		std::unique_ptr<sqlite3, Deleter> mSQLitePtr;   ///< Pointer to SQLite Database Connection Handle
		std::string mFilename;                          ///< UTF-8 filename used to open the database
	};

