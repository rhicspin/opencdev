/* vim: set sw=3: */

#include "opencdev.h"
#include "LocalLogReader_p.h"

namespace opencdev {

LocalLogReader::LocalLogReader(const string &base_path)
   : d(new LocalLogReaderPrivate(base_path))
{
   // Nothing
}

LocalLogReader::LocalLogReader(const LocalLogReader &other)
   : d(new LocalLogReaderPrivate(*other.d))
{
   // Nothing
}

LocalLogReader&     LocalLogReader::operator=(const LocalLogReader &other)
{
   if(this != &other)
   {
      *d = *(other.d);
   }
   return *this;
}

LocalLogReader::~LocalLogReader()
{
   delete d;
}

void    LocalLogReader::query_fill(const string &logger, int fill_id, result_t *result)
{
   d->query_fill(logger, fill_id, result);
}

void    LocalLogReader::query_timerange(const string &logger, cdev_time_t starttime, cdev_time_t endtime, result_t *result)
{
   d->query_timerange(logger, starttime, endtime, result);
}

}
