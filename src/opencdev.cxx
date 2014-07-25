/* vim: set sw=3: */

#include <boost/filesystem.hpp>
#include <boost/date_time/local_time/local_time.hpp>
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

void    DB::read_sdds_files(const vector<file_rec_t> &files, result_t *result, cdev_time_t starttime, cdev_time_t endtime)
{
   for(vector<file_rec_t>::const_iterator it = files.begin(); it != files.end(); it++)
   {
      read_sdds_file(*it, result, starttime, endtime);
   }
}

cdev_time_t    DB::from_utc(cdev_time_t utc_time)
{
   using namespace boost::local_time;
   using namespace boost::posix_time;
   const double ticks_per_second =
      boost::posix_time::time_duration::rep_type::ticks_per_second;
   // Define time zone
   static time_zone_ptr ny_tz(new posix_time_zone("EST-5EDT,M3.2.0,M11.1.0"));
   // Convert from UNIX time
   static ptime unix_epoch(boost::gregorian::date(1970,1,1));
   time_duration dt(0, 0, 0, ticks_per_second * utc_time);
   ptime utc_ptime = unix_epoch + dt;
   // Convert to NY time zone
   local_date_time ny_time(utc_ptime, ny_tz);
   // Convert to UNIX time
   ptime ny_ptime = ny_time.local_time();
   return (ny_ptime - unix_epoch).ticks() / ticks_per_second;
}

void    DB::read_sdds_file(const file_rec_t &file, result_t *result, cdev_time_t starttime, cdev_time_t endtime)
{
   const string &orig_path = file.path;
   if (orig_path.find(CAD_SDDS_BASE) != 0)
   {
      throw "Unexpected SDDS base path";
   }
   string rel_path = orig_path.substr(CAD_SDDS_BASE.size());
   string filepath = get_sdds_path(rel_path);
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
   const cdev_time_t file_starttime = f.getParameterInDouble(const_cast<char*>("FileStartTime"), page_id);
   const double hole_value = f.getParameterInDouble(const_cast<char*>("HoleValue"), page_id);
   const int32_t column_count = f.getColumnCount();
   const int32_t row_count = f.rowCount(page_id);
   double *time = f.getColumnInDouble(0, page_id);
   if (fabs(from_utc(file_starttime) - file.timestamp) > 60*30)
   {
      throw "Inconsistent FileStartTime. Wrong time zone?";
   }
   for(int32_t col = 1; col < column_count; col++)
   {
      double *values = f.getColumnInDouble(col, page_id);
      map<cdev_time_t, double> &col_result = (*result)[f.getColumnName(col)];

      for(int32_t row = 0; row < row_count; row++)
      {
         cdev_time_t row_time = from_utc(time[row] + file_starttime);
         if (((starttime == 0) && (endtime == 0)) || ((starttime <= row_time) && (endtime >= row_time)))
         {
            double value = values[row];
            if (value == hole_value)
            {
               continue;
            }
            col_result[row_time] = value;
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
   vector<file_rec_t> files = _db.get_fill_files(get_logreq_path(logger), fill_id);
   
   read_sdds_files(files, result, 0, 0);
}

void    DB::query_timerange(const string &logger, cdev_time_t starttime, cdev_time_t endtime, result_t *result)
{
   vector<file_rec_t> files = _db.get_timerange_files(get_logreq_path(logger), starttime, endtime);

   read_sdds_files(files, result, starttime, endtime);
}

}
