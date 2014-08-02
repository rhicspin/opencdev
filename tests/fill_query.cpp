/* vim: set sw=3: */

#include <string>
#include <map>
#include <math.h>

#include <boost/test/unit_test.hpp>

#include <opencdev.h>

using namespace std;

BOOST_AUTO_TEST_SUITE(fill_query)

BOOST_AUTO_TEST_CASE(check_load)
{
   opencdev::DB db(TEST_DATA_PATH);
   opencdev::result_t result;

   db.query_fill("RHIC/Polarimeter/Yellow/biasReadbacks.logreq", 17201, &result);

   /*
    * Obtained by running
    * exportLoggerData \
    *  -logger 'RHIC/Polarimeter/Yellow/biasReadbacks' \
    *  -timeformat 'unix' -showmissingdatawith x \
    *  -fill 17201
    */

   const int reference_column_count = 12;
   const int reference_row_count = 113;
   const double reference_time = 1362915002.143;
   const char* reference_names[reference_column_count] =
   {
      "yo12-pol3.1-det1.i:currentM",
      "yo12-pol3.1-det2.i:currentM",
      "yo12-pol3.1-det3.i:currentM",
      "yo12-pol3.1-det4.i:currentM",
      "yo12-pol3.1-det5.i:currentM",
      "yo12-pol3.1-det6.i:currentM",
      "yo12-pol3.2-det1.i:currentM",
      "yo12-pol3.2-det2.i:currentM",
      "yo12-pol3.2-det3.i:currentM",
      "yo12-pol3.2-det4.i:currentM",
      "yo12-pol3.2-det5.i:currentM",
      "yo12-pol3.2-det6.i:currentM"
   };
   const double reference_values[reference_column_count] =
   {
      -3.07599, -3.29505, -2.32856, -7.05455, -10.5984, -2.62332,
      -0.973748, -11.2269, -8.84616, -10.3479, -16.1589, -6.07836
   };
   const double epsilon = 0.001;

   BOOST_CHECK_EQUAL(result.size(), reference_column_count);

   for(opencdev::result_t::const_iterator it = result.begin(); it != result.end(); it++)
   {
      const string &cell_name = it->first;
      const map<opencdev::cdev_time_t, double> &values = it->second;

      BOOST_CHECK_EQUAL(values.size(), reference_row_count);

      bool reference_time_encountered = false;

      // Scan each column
      for(map<opencdev::cdev_time_t, double>::const_iterator row_it = values.begin();
            row_it != values.end(); row_it++)
      {
         const opencdev::cdev_time_t time = row_it->first;
         const double value = row_it->second;

         // In this test we check only one row
         if (fabs(time - reference_time) < epsilon)
         {
            reference_time_encountered = true;

            int cell_id = -1;
            for(int i = 0; i < reference_column_count; i++)
            {
               if (cell_name == reference_names[i])
               {
                  cell_id = i;
                  break;
               }
            }
            BOOST_CHECK(cell_id >= 0);
            BOOST_CHECK(fabs(value - reference_values[cell_id]) < epsilon);
         }
      }
      BOOST_CHECK(reference_time_encountered);
   }
}

BOOST_AUTO_TEST_SUITE_END()
