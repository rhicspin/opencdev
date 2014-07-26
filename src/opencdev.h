/* vim: set sw=3: */

#ifndef _OPENCDEV_H
#define _OPENCDEV_H

#include <map>
#include <string>

namespace opencdev {

typedef double cdev_time_t;
typedef std::map<std::string, std::map<cdev_time_t, double> > result_t;

}

#include "DB.h"

#endif
