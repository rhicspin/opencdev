/* vim: set sw=3: */

#include "opencdev.h"
#include "DB_p.h"

namespace opencdev {

DB::DB(const string &base_path)
   : d(new DBPrivate(base_path))
{
   // Nothing
}

DB::DB(const DB &other)
   : d(new DBPrivate(*other.d))
{
   // Nothing
}

DB&     DB::operator=(const DB &other)
{
   if(this != &other)
   {
      *d = *(other.d);
   }
   return *this;
}

DB::~DB()
{
   delete d;
}

void    DB::query_fill(const string &logger, int fill_id, result_t *result)
{
   d->query_fill(logger, fill_id, result);
}

void    DB::query_timerange(const string &logger, cdev_time_t starttime, cdev_time_t endtime, result_t *result)
{
   d->query_timerange(logger, starttime, endtime, result);
}

}
