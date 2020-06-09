#include "Sqlit3Db.h"
#include "Exception.h"

namespace SQLite {

	const int   OPEN_READONLY = SQLITE_OPEN_READONLY;
	const int   OPEN_READWRITE = SQLITE_OPEN_READWRITE;
	const int   OPEN_CREATE = SQLITE_OPEN_CREATE;
	const int   OPEN_URI = SQLITE_OPEN_URI;

	const int   OK = SQLITE_OK;

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
			throw SQLite::Exception(handle, ret);
		}
		if (aBusyTimeoutMs > 0)
		{
			const int ret = sqlite3_busy_timeout(getHandle(), aBusyTimeoutMs);
			check(ret);
		}
	}

	Document Sqlit3Db::retrieve(string tablename, Document params, vector<string> fields)
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

}
