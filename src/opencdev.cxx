/* vim: set sw=3: */

#include <boost/filesystem.hpp>
#include <SDDS3.h>

#include "opencdev.h"
#include "SqliteDriver.h"

namespace fs = boost::filesystem;

namespace opencdev {

string  DB::get_logreq_path(const string &logger)
{
   return (fs::path(CAD_LOGREQ_BASE)/fs::path(logger)).string();
}

string  DB::get_sdds_path(const string &rel_path)
{
   return (fs::path(_base_path)/fs::path(rel_path)).string();
}

void    DB::read_sdds_files(const vector<string> &files, result_t *result, int64_t starttime, int64_t endtime)
{
   for(vector<string>::const_iterator it = files.begin(); it != files.end(); it++)
   {
      const string &path = *it;
      if (path.find(CAD_SDDS_BASE) == 0)
      {
         string rel_path = path.substr(CAD_SDDS_BASE.size());

         read_sdds_file(get_sdds_path(rel_path), result, starttime, endtime);
      }
   }
}

void    DB::read_sdds_file(const string &filepath, result_t *result, int64_t starttime, int64_t endtime)
{
   if (!fs::exists(filepath))
   {
      throw "File not found";
   }
   SDDSFile f(const_cast<char*>(filepath.c_str()));
   f.gzipFile = true;
   if (f.readFile() != 0)
   {
      f.printErrors();
      throw "Error reading SDDS file";
   }

   assert(f.pageCount() == 1);
   assert(f.getColumnIndex(const_cast<char*>("Time")) == 0);
   const int32_t page_id = 1;
   const int64_t file_starttime = f.getParameterInDouble(const_cast<char*>("FileStartTime"), page_id);
   const double hole_value = f.getParameterInDouble(const_cast<char*>("HoleValue"), page_id);
   const int32_t column_count = f.getColumnCount();
   const int32_t row_count = f.rowCount(page_id);
   double *time = f.getColumnInDouble(0, page_id);
   for(int32_t col = 1; col < column_count; col++)
   {
      double *values = f.getColumnInDouble(col, page_id);
      map<cdev_time_t, double> &col_result = (*result)[f.getColumnName(col)];

      for(int32_t row = 0; row < row_count; row++)
      {
         cdev_time_t row_time = time[row];
         if (((starttime == 0) && (endtime == 0)) || ((starttime < row_time) && (endtime > row_time)))
         {
            double value = values[row];
            if (value == hole_value)
            {
               continue;
            }
            col_result[row_time + file_starttime] = value;
         }
      }
   }
}

DB::DB(const string &base_path)
   : _base_path(base_path)
   , _db((fs::path(_base_path)/"db.sqlite").string())
{
   // Nothing
}

void    DB::query_fill(const string &logger, int fill_id, result_t *result)
{
   vector<string> files = _db.get_fill_files(get_logreq_path(logger), fill_id);
   
   read_sdds_files(files, result, 0, 0);
}

void    DB::query_timerange(const string &logger, int64_t starttime, int64_t endtime, result_t *result)
{
   vector<string> files = _db.get_timerange_files(get_logreq_path(logger), starttime, endtime);

   read_sdds_files(files, result, starttime, endtime);
}

}
