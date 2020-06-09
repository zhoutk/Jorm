#pragma once
#include "Idb.h"
#include "sqlit3/sqlite3.h"

#define SQLITECPP_ASSERT(expression, message)   assert(expression && message)

namespace SQLite {

	extern const int OPEN_READONLY;     // SQLITE_OPEN_READONLY
	extern const int OPEN_READWRITE;    // SQLITE_OPEN_READWRITE
	extern const int OPEN_CREATE;       // SQLITE_OPEN_CREATE
	extern const int OPEN_URI;          // SQLITE_OPEN_URI

	extern const int OK;                ///< SQLITE_OK (used by check() bellow)

	class Sqlit3Db : public Idb
	{
		Sqlit3Db(const char* apFilename,
			const int   aFlags = SQLite::OPEN_READONLY,
			const int   aBusyTimeoutMs = 0,
			const char* apVfs = nullptr);

		Sqlit3Db(const std::string& aFilename,
			const int          aFlags = SQLite::OPEN_READONLY,
			const int          aBusyTimeoutMs = 0,
			const std::string& aVfs = "") :
			Sqlit3Db(aFilename.c_str(), aFlags, aBusyTimeoutMs, aVfs.empty() ? nullptr : aVfs.c_str())
		{}

		Sqlit3Db(const Sqlit3Db&) = delete;
		Sqlit3Db& operator=(const Sqlit3Db&) = delete;

		Sqlit3Db(Sqlit3Db&& aDatabase) = default;
		Sqlit3Db& operator=(Sqlit3Db&& aDatabase) = default;

		~Sqlit3Db() = default;

		struct Deleter
		{
			void operator()(sqlite3* apSQLite);
		};

	public:
		Document retrieve(string tablename, Document params, vector<string> fields = vector<string>());

		sqlite3* getHandle() const noexcept
		{
			return mSQLitePtr.get();
		}

		void check(const int aRet) const
		{
			if (SQLite::OK != aRet)
			{
				throw SQLite::Exception(getHandle(), aRet);
			}
		}

	private:
		// TODO: perhaps switch to having Statement sharing a pointer to the Connexion
		std::unique_ptr<sqlite3, Deleter> mSQLitePtr;   ///< Pointer to SQLite Database Connection Handle
		std::string mFilename;                          ///< UTF-8 filename used to open the database
	};

}