/* vim: set sw=3: */

#ifndef _LOG_READER_H
#define _LOG_READER_H

#include <string>

#include "opencdev.h"

namespace opencdev {

/**
 * Base class for log reader implementation.
 */
class LogReader
{
private:

   void calculate_mean(const result_t &result, mean_result_t *mean_result_t) const;

public:

   virtual void query_fill(const std::string &logger, int fill_id, result_t *result);
   /**
    * @param starttime  UNIX timestamp in UTC
    * @param endtime    UNIX timestamp in UTC
    */
   virtual void query_timerange(const std::string &logger, cdev_time_t starttime, cdev_time_t endtime, result_t *result);
   virtual void query_fill_mean(const std::string &logger, int fill_id, mean_result_t *mean_result);
   /**
    * @param starttime  UNIX timestamp in UTC
    * @param endtime    UNIX timestamp in UTC
    */
   virtual void query_timerange_mean(const std::string &logger, cdev_time_t starttime, cdev_time_t endtime, mean_result_t *mean_result);
};

}

#endif
