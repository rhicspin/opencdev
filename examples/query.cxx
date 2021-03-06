/* vim: set sw=3: */

#include <stdlib.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <map>

#include <opencdev.h>

using std::map;
using std::string;
using std::cout;
using std::endl;

int main(void)
{
   opencdev::LocalLogReader log_reader("/star/u/veprbl/run_fy13");
   opencdev::result_t result;
   log_reader.query_fill("RHIC/Polarimeter/Blue/biasReadbacks", 17104, &result);

   cout << std::fixed << std::setprecision(1);

   cout << "Time\t";
   for(opencdev::result_t::const_iterator it = result.begin(); it != result.end(); it++)
   {
      const string &cell_name = it->first;
      const map<opencdev::cdev_time_t, double> &values = it->second;
      cout << cell_name << "\t";
   }
   cout << endl;

   if (result.size() > 0)
   {
      const map<opencdev::cdev_time_t, double> &time_map = result.begin()->second;
      for(map<opencdev::cdev_time_t, double>::const_iterator tit = time_map.begin(); tit != time_map.end(); tit++)
      {
         opencdev::cdev_time_t time = tit->first;
         cout << time << "\t";
         for(opencdev::result_t::const_iterator it = result.begin(); it != result.end(); it++)
         {
            const string &cell_name = it->first;
            const map<opencdev::cdev_time_t, double> &values = it->second;
            cout << values.at(time) << "\t";
         }
         cout << endl;
      }
   }

   return EXIT_SUCCESS;
}
