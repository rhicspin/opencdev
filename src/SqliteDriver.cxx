/* vim: set sw=3: */

#include <sqlite3.h>

#include "SqliteDriver.h"

namespace opencdev {

SqliteDriver::SqliteDriver(const string &db_file)
{
   if (sqlite3_open_v2(db_file.c_str(), &_db, SQLITE_OPEN_READONLY, NULL) != SQLITE_OK)
   {
      throw "Cannot open sqlite3 database";
   }
   int ret;
   // TODO: Check if we should use stopFillNo as well
   char fill_metadata_query[] = "SELECT filePath FROM rhicFileHeaderV WHERE requestFile = ? AND fillNo = ?;";
   ret = sqlite3_prepare_v2(_db, fill_metadata_query, sizeof(fill_metadata_query), &_fill_query_stmt, NULL);
   if (ret != SQLITE_OK)
   {
      throw "Cannot prepare sql";
   }
   char timerange_metadata_query[] = "SELECT filePath FROM rhicFileHeaderV WHERE requestFile = ? AND timeStamp <= datetime(?, 'unixepoch') AND ((stopTimeStamp = NULL) OR (stopTimeStamp >= datetime(?, 'unixepoch'))) ORDER BY timeStamp;";
   ret = sqlite3_prepare_v2(_db, timerange_metadata_query, sizeof(timerange_metadata_query), &_timerange_query_stmt, NULL);
   if (ret != SQLITE_OK)
   {
      throw "Cannot prepare sql";
   }
}

SqliteDriver::~SqliteDriver()
{
   sqlite3_close(_db);
}

vector<string> SqliteDriver::get_file_list(sqlite3_stmt *stmt)
{
   vector<string> result;

   int ret;
   do
   {
      ret = sqlite3_step(stmt);
      if (ret == SQLITE_ROW)
      {
         result.push_back((const char*)sqlite3_column_text(stmt, 0));
      }
      else if (ret != SQLITE_DONE)
      {
         throw sqlite3_errmsg(_db);
      }
   }
   while (ret != SQLITE_DONE);

   return result;
}

vector<string> SqliteDriver::get_fill_files(const string &logreq_path, int fill_id)
{
   sqlite3_reset(_fill_query_stmt);
   sqlite3_bind_text(_fill_query_stmt, 1, logreq_path.c_str(), logreq_path.size(), SQLITE_STATIC);
   sqlite3_bind_int(_fill_query_stmt, 2, fill_id);

   return get_file_list(_fill_query_stmt);
}

vector<string> SqliteDriver::get_timerange_files(const string &logreq_path, int64_t starttime, int64_t endtime)
{
   sqlite3_reset(_timerange_query_stmt);
   sqlite3_bind_text(_timerange_query_stmt, 1, logreq_path.c_str(), logreq_path.size(), SQLITE_STATIC);
   sqlite3_bind_int64(_timerange_query_stmt, 2, endtime);
   sqlite3_bind_int64(_timerange_query_stmt, 3, starttime);

   return get_file_list(_timerange_query_stmt);
}


}
