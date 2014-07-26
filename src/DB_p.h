/* vim: set sw=3: */

#ifndef _DB_P_H
#define _DB_P_H

#include <string>

#include "opencdev.h"
#include "SqliteDriver.h"

using std::string;

namespace opencdev {

const string CAD_SDDS_BASE = "/operations/app_store/RunData/run_fy13";
const string CAD_LOGREQ_BASE = "/operations/app_store/Gpm";

class DBPrivate
{
private:

   string       _base_path;
   SqliteDriver _db;

   string get_logreq_path(const string &logger);
   string get_sdds_path(const string &rel_path);
   cdev_time_t from_utc(cdev_time_t time);
   void   read_sdds_files(const vector<file_rec_t> &files, result_t *result, cdev_time_t starttime, cdev_time_t endtime);
   void   read_sdds_file(const file_rec_t &file, result_t *result, cdev_time_t starttime, cdev_time_t endtime);

public:

   DBPrivate(const string &base_path);
   void query_fill(const string &logger, int fill_id, result_t *result);
   void query_timerange(const string &logger, cdev_time_t starttime, cdev_time_t endtime, result_t *result);
};

}

#endif
