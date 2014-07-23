/* vim: set sw=3: */

#ifndef _SQLITE_DRIVER_H
#define _SQLITE_DRIVER_H

#include <stdint.h>
#include <string>
#include <vector>

#include <sqlite3.h>

using std::string;
using std::vector;

namespace opencdev {

class SqliteDriver
{
   private:

      sqlite3           *_db;
      sqlite3_stmt      *_fill_query_stmt, *_timerange_query_stmt;

      vector<string>    get_file_list(sqlite3_stmt *stmt);

   public:

      SqliteDriver(const string &db_file);
      ~SqliteDriver();
      vector<string>    get_fill_files(const string &logreq_path, int fill_id);
      vector<string>    get_timerange_files(const string &logreq_path, int64_t starttime, int64_t endtime);
};

}

#endif
