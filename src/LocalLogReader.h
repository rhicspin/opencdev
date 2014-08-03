/* vim: set sw=3: */

#ifndef _LOCAL_LOG_READER_H
#define _LOCAL_LOG_READER_H

#include <string>

#include "opencdev.h"
#include "LogReader.h"

namespace opencdev {

class LocalLogReaderPrivate;
class LocalLogReader : public LogReader
{
private:

   LocalLogReaderPrivate* d;

public:

   LocalLogReader(const std::string &base_path);
   LocalLogReader(const LocalLogReader&);
   LocalLogReader&  operator=(const LocalLogReader&);
   virtual ~LocalLogReader();
   virtual void query_fill(const std::string &logger, int fill_id, result_t *result);
   virtual void query_timerange(const std::string &logger, cdev_time_t starttime, cdev_time_t endtime, result_t *result);
};

}

#endif
