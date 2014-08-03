#include <cmath>
#include <map>
#include <numeric>
#include <string>

#include "opencdev.h"
#include "LogReader.h"

namespace opencdev {

template<typename K, typename V>
V val_add(V sum, const std::pair<K, V> &p)
{
   return sum + p.second;
} 

void LogReader::calculate_mean(const result_t &result, mean_result_t *mean_result) const
{
   for (result_t::const_iterator it = result.begin(); it != result.end(); it++)
   {
      const std::string &key = it->first;
      const std::map<cdev_time_t, double> &m = it->second;
      double mean = NAN;

      if (m.size() != 0)
      {
         mean = std::accumulate(m.begin(), m.end(), 0.0f, val_add<cdev_time_t, double>) / m.size();
      }

      (*mean_result)[key] = mean;
   }
}

void LogReader::query_fill(const std::string &logger, int fill_id, result_t *result)
{
   throw "Undefined method";
}

void LogReader::query_timerange(const std::string &logger, cdev_time_t starttime, cdev_time_t endtime, result_t *result)
{
   throw "Undefined method";
}

void LogReader::query_fill_mean(const std::string &logger, int fill_id, mean_result_t *mean_result)
{
   result_t result;
   query_fill(logger, fill_id, &result);
   calculate_mean(result, mean_result);
}

void LogReader::query_timerange_mean(const std::string &logger, cdev_time_t starttime, cdev_time_t endtime, mean_result_t *mean_result)
{
   result_t result;
   query_timerange(logger, starttime, endtime, &result);
   calculate_mean(result, mean_result);
}

}
