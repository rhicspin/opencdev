/* vim: set sw=3: */

#ifndef _LOCAL_LOG_READER_P_H
#define _LOCAL_LOG_READER_P_H

#include <string>

#include <SDDS.h>

#include "opencdev.h"
#include "SqliteDriver.h"

using std::string;

namespace opencdev {

const string CAD_SDDS_BASE = "/operations/app_store/RunData";
const string CAD_LOGREQ_BASE = "/operations/app_store/Gpm";

class LocalLogReaderPrivate
{
private:

   string       _base_path;
   SqliteDriver _db;

   string get_logreq_path(const string &logger);
   string get_sdds_path(const string &rel_path);
   cdev_time_t to_utc(cdev_time_t ny_time);
   vector<int> make_time_column_map(char **col_name_arr, SDDS_DATASET *SDDS_dataset_ptr);
   void   read_sdds_files(const vector<file_rec_t> &files, result_t *result, cdev_time_t starttime, cdev_time_t endtime);
   void   read_sdds_file(const file_rec_t &file, result_t *result, cdev_time_t starttime, cdev_time_t endtime);

public:

   LocalLogReaderPrivate(const string &base_path);
   void query_fill(const string &logger, int fill_id, result_t *result);
   void query_timerange(const string &logger, cdev_time_t starttime, cdev_time_t endtime, result_t *result);
};

}

#endif
