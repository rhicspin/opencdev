/* vim: set sw=3: */

#ifndef _DB_H
#define _DB_H

#include <string>

namespace opencdev {

class DBPrivate;
class DB
{
private:

   DBPrivate* d;
public:

   DB(const std::string &base_path);
   DB(const DB&);
   DB&  operator=(const DB&);
   ~DB();
   void query_fill(const std::string &logger, int fill_id, result_t *result);
   /**
    * @param starttime  UNIX timestamp in NYC timezone
    * @param endtime    UNIX timestamp in NYC timezone
    */
   void query_timerange(const std::string &logger, cdev_time_t starttime, cdev_time_t endtime, result_t *result);
};

}

#endif
