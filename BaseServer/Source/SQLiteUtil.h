#ifndef BASESERVER_SQLITEUTIL_HEADER_
#define BASESERVER_SQLITEUTIL_HEADER_

#include <SQLiteCpp/Database.h>

#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

typedef boost::shared_ptr<SQLite::Database> DatabasePtr;


inline std::string sqlText(const std::string& s)
{
	return "'" + boost::replace_all_copy(s, "'", "''") + "'";
}

#endif // BASESERVER_SQLITEUTIL_HEADER_

