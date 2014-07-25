/* vim: set sw=3: */

#ifndef _OPENCDEV_H
#define _OPENCDEV_H

#include <string>
#include <vector>
#include <map>

#include "SqliteDriver.h"

using std::string;
using std::vector;
using std::map;

namespace opencdev {

typedef double cdev_time_t;
typedef map<string, map<cdev_time_t, double> > result_t;

const string CAD_SDDS_BASE = "/operations/app_store/RunData/run_fy13";
const string CAD_LOGREQ_BASE = "/operations/app_store/Gpm";

class DB
{
private:

   string       _base_path;
   SqliteDriver _db;

   string get_logreq_path(const string &logger);
   string get_sdds_path(const string &rel_path);
   cdev_time_t from_utc(cdev_time_t time);
   void   read_sdds_files(const vector<file_rec_t> &files, result_t *result, int64_t starttime, int64_t endtime);
   void   read_sdds_file(const file_rec_t &file, result_t *result, int64_t starttime, int64_t endtime);

public:

   DB(const string &base_path);
   void query_fill(const string &logger, int fill_id, result_t *result);
   /**
    * @param starttime  UNIX timestamp in NYC timezone
    * @param endtime    UNIX timestamp in NYC timezone
    */
   void query_timerange(const string &logger, int64_t starttime, int64_t endtime, result_t *result);
};

}

#endif
