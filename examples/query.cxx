/* vim: set sw=3: */

#include <stdlib.h>
#include <string>
#include <iostream>

#include <opencdev.h>

using std::string;
using std::cout;
using std::endl;

int main(void)
{
   opencdev::DB db("/star/u/veprbl/run_fy13");
   opencdev::result_t result;
   db.query_fill("RHIC/Polarimeter/Blue/biasReadbacks.logreq", 17104, &result);

   for(opencdev::result_t::const_iterator it = result.begin(); it != result.end(); it++)
   {
      const string &cell_name = it->first;
      const map<opencdev::cdev_time_t, double> &values = it->second;
      cout << cell_name << endl;
   }

   return EXIT_SUCCESS;
}
