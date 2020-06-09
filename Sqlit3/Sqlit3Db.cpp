#include "Sqlit3Db.h"

	Sqlit3Db::Sqlit3Db(const char* apFilename,
		const int aFlags,
		const int aBusyTimeoutMs,
		const char* apVfs) :
		mFilename(apFilename)
	{
		sqlite3* handle;
		const int ret = sqlite3_open_v2(apFilename, &handle, aFlags, apVfs);
		mSQLitePtr.reset(handle);
		if (SQLITE_OK != ret)
		{
			throw "SQLite Error, return code: " + ret;
		}
		if (aBusyTimeoutMs > 0)
		{
			const int ret = sqlite3_busy_timeout(getHandle(), aBusyTimeoutMs);
			check(ret);
		}
	}

	Sqlit3Db::~Sqlit3Db()
	{
	}

	Document Sqlit3Db::retrieve(string tablename, Document *params, vector<string> fields)
	{
		return Document();
	}

	void Sqlit3Db::Deleter::operator()(sqlite3* apSQLite)
	{
		const int ret = sqlite3_close(apSQLite); // Calling sqlite3_close() with a nullptr argument is a harmless no-op.

		// Avoid unreferenced variable warning when build in release mode
		(void)ret;

		// Only case of error is SQLITE_BUSY: "database is locked" (some statements are not finalized)
		// Never throw an exception in a destructor :
		SQLITECPP_ASSERT(SQLITE_OK == ret, "database is locked");  // See SQLITECPP_ENABLE_ASSERT_HANDLER
	}

