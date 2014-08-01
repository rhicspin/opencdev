/* vim: set sw=3: */

#include <map>

#include <boost/filesystem.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <SDDS3.h>

#include "opencdev.h"
#include "DB_p.h"
#include "SqliteDriver.h"

using std::map;

namespace fs = boost::filesystem;

namespace opencdev {

string  DBPrivate::get_logreq_path(const string &logger)
{
   return (fs::path(CAD_LOGREQ_BASE)/fs::path(logger)).string();
}

string  DBPrivate::get_sdds_path(const string &rel_path)
{
   return (fs::path(_base_path)/fs::path(rel_path)).string();
}

void    DBPrivate::read_sdds_files(const vector<file_rec_t> &files, result_t *result, cdev_time_t starttime, cdev_time_t endtime)
{
   for(vector<file_rec_t>::const_iterator it = files.begin(); it != files.end(); it++)
   {
      read_sdds_file(*it, result, starttime, endtime);
   }
}

cdev_time_t    DBPrivate::to_utc(cdev_time_t ny_time)
{
   using namespace boost::local_time;
   using namespace boost::posix_time;
   const double ticks_per_second =
      boost::posix_time::time_duration::rep_type::ticks_per_second;
   // Define time zone
   static time_zone_ptr ny_tz(new posix_time_zone("EST-5EDT,M3.2.0,M11.1.0"));
   // Convert from UNIX time
   static ptime unix_epoch(boost::gregorian::date(1970,1,1));
   time_duration dt(0, 0, 0, ticks_per_second * ny_time);
   ptime ny_ptime = unix_epoch + dt;
   // Convert to NY time zone
   local_date_time date_time(ny_ptime.date(), ny_ptime.time_of_day(), ny_tz,
         local_date_time::EXCEPTION_ON_ERROR);
   // Convert to UNIX time
   ptime utc_ptime = date_time.utc_time();
   return (utc_ptime - unix_epoch).ticks() / ticks_per_second;
}

/**
 * Make map from value to it's corresponding measurement time.
 *
 * For, example, bi12-pol3.1-det1.i:currentM maps to a bi12-pol3.1-det1.i:currentMT column
 *
 * This should work when sddsIOFileType == "SDDSIO_LOG_VTf"
 */
vector<int>    DBPrivate::make_time_column_map(SDDSFile &f)
{
   const int32_t column_count = f.getColumnCount();
   vector<int> result;
   result.resize(column_count);

   for(int32_t col = 1; col < column_count; col++) // start at 1 to skip time column
   {
      string col_name = f.getColumnName(col);
      if (col_name[col_name.size()-1] == 'T')
      {
         string col_units = f.getColumnUnits(col);
         assert(col_units == "seconds");
         result[col] = -1; // mark column for skipping
         continue;
      }
      string time_col_name = col_name + "T";
      int col_id = f.getColumnIndex(const_cast<char*>(time_col_name.c_str()));
      if (col_id >= 0)
      {
         result[col] = col_id;
      }
      else
      {
         // use recorded time column by default
         result[col] = 0;
      }
   }

   return result;
}

void    DBPrivate::read_sdds_file(const file_rec_t &file, result_t *result, cdev_time_t starttime, cdev_time_t endtime)
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
   const double utc_starttime = to_utc(starttime);
   const double utc_endtime = to_utc(endtime);
   const double hole_value = f.getParameterInDouble(const_cast<char*>("HoleValue"), page_id);
   const int32_t column_count = f.getColumnCount();
   const int32_t row_count = f.rowCount(page_id);
   if (fabs(file_starttime - to_utc(file.timestamp)) > 60*30)
   {
      throw "Inconsistent FileStartTime. Wrong time zone?";
   }

   vector<int> time_col_map = make_time_column_map(f);

   for(int32_t col = 1; col < column_count; col++) // start at 1 to skip time column
   {
      int32_t time_col_index = time_col_map[col];
      if (time_col_index < 0)
      {
         continue;
      }
      double *time = f.getColumnInDouble(time_col_index, page_id);
      double *values = f.getColumnInDouble(col, page_id);
      map<cdev_time_t, double> &col_result = (*result)[f.getColumnName(col)];

      for(int32_t row = 0; row < row_count; row++)
      {
         cdev_time_t row_time = time[row] + file_starttime;
         if (((starttime == 0) && (endtime == 0)) || ((utc_starttime <= row_time) && (utc_endtime >= row_time)))
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

DBPrivate::DBPrivate(const string &base_path)
   : _base_path(base_path)
   , _db((fs::path(_base_path)/"db.sqlite").string())
{
   // Nothing
}

void    DBPrivate::query_fill(const string &logger, int fill_id, result_t *result)
{
   vector<file_rec_t> files = _db.get_fill_files(get_logreq_path(logger), fill_id);
   
   read_sdds_files(files, result, 0, 0);
}

void    DBPrivate::query_timerange(const string &logger, cdev_time_t starttime, cdev_time_t endtime, result_t *result)
{
   vector<file_rec_t> files = _db.get_timerange_files(get_logreq_path(logger), starttime, endtime);

   read_sdds_files(files, result, starttime, endtime);
}

}
