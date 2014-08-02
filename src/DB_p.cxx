/* vim: set sw=3: */

#include <map>
#include <cstring>
#include <cmath>

#include <boost/scoped_array.hpp>
#include <boost/filesystem.hpp>

#include <SDDS.h>

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

/**
 * Make map from value to it's corresponding measurement time.
 *
 * For, example, bi12-pol3.1-det1.i:currentM maps to a bi12-pol3.1-det1.i:currentMT column
 *
 * This should work when sddsIOFileType == "SDDSIO_LOG_VTf"
 */
vector<int>    DBPrivate::make_time_column_map(char **col_name_arr, SDDS_DATASET *SDDS_dataset_ptr)
{
   const int32_t column_count = SDDS_ColumnCount(SDDS_dataset_ptr);
   vector<int> result;
   result.resize(column_count);

   for(int32_t col = 1; col < column_count; col++) // start at 1 to skip time column
   {
      string col_name = col_name_arr[col];
      if (col_name[col_name.size()-1] == 'T')
      {
         result[col] = -1; // mark column for skipping
         continue;
      }
      string time_col_name = col_name + "T";
      int col_id = SDDS_GetColumnIndex(SDDS_dataset_ptr, const_cast<char*>(time_col_name.c_str()));
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

   SDDS_DATASET SDDS_dataset;
   int32_t ret = SDDS_InitializeInput(&SDDS_dataset, const_cast<char*>(filepath.c_str()));
   if (!ret)
   {
      SDDS_PrintErrors(stderr, 1);
      throw "Error reading SDDS file";
   }

   int page_count = SDDS_ReadPage(&SDDS_dataset);
   assert(page_count == 1);

   double file_starttime, hole_value;
   SDDS_GetParameterAsDouble(&SDDS_dataset, const_cast<char*>("FileStartTime"), &file_starttime);
   SDDS_GetParameterAsDouble(&SDDS_dataset, const_cast<char*>("HoleValue"), &hole_value);

   if (fabs(file_starttime - file.timestamp) > 60*30)
   {
      throw "Inconsistent FileStartTime. Wrong time zone?";
   }

   const int32_t row_count = SDDS_RowCount(&SDDS_dataset);
   int32_t column_count;
   char **col_name_arr = SDDS_GetColumnNames(&SDDS_dataset, &column_count);

   assert(strcmp(col_name_arr[0], "Time") == 0);

   vector<int> time_col_map = make_time_column_map(col_name_arr, &SDDS_dataset);

   for(int32_t col = 1; col < column_count; col++) // start at 1 to skip time column
   {
      int32_t time_col_index = time_col_map[col];
      if (time_col_index < 0)
      {
         continue;
      }
      char *col_name = col_name_arr[col];
      boost::scoped_array<double> time(SDDS_GetColumnInDoubles(&SDDS_dataset, col_name_arr[time_col_index]));
      boost::scoped_array<double> values(SDDS_GetColumnInDoubles(&SDDS_dataset, col_name));
      map<cdev_time_t, double> &col_result = (*result)[col_name];

      for(int32_t row = 0; row < row_count; row++)
      {
         cdev_time_t row_time = time[row] + file_starttime;
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
      SDDS_Free(col_name);
   }

   SDDS_Free(col_name_arr);
   SDDS_Terminate(&SDDS_dataset);
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
