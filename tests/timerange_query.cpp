/* vim: set sw=3: */

#include <string>
#include <map>
#include <math.h>

#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <opencdev.h>

using namespace std;

BOOST_AUTO_TEST_SUITE(timerange_query)

BOOST_AUTO_TEST_CASE(check_load_fail)
{
   BOOST_CHECK_THROW(opencdev::DB db("/this/does/not/exist"), const char*); 
}

BOOST_AUTO_TEST_CASE(check_load)
{
   opencdev::DB db(TEST_DATA_PATH);
   opencdev::result_t result;

   using namespace boost::posix_time;
   using namespace boost::gregorian;

   ptime t1(date(2013, Mar, 10), time_duration(9, 15, 0));
   ptime t2(date(2013, Mar, 10), time_duration(9, 16, 0));
   const double ticks_per_second =
      time_duration::rep_type::ticks_per_second;
   ptime unix_epoch(boost::gregorian::date(1970,1,1));
   double t1v = (t1 - unix_epoch).ticks() / ticks_per_second;
   double t2v = (t2 - unix_epoch).ticks() / ticks_per_second;

   db.query_timerange("RHIC/Polarimeter/Yellow/biasReadbacks.logreq", t1v, t2v, &result);

   /*
    * Obtained by running
    * exportLoggerData \
    *  -logger 'RHIC/Polarimeter/Yellow/biasReadbacks' \
    *  -timeformat 'unix' -showmissingdatawith x \
    *  -start 'Sun Mar 10 09:15:00 2013' -stop 'Sun Mar 10 09:16:00 2013'
    */
   const int reference_size = 12;
   const double reference_time = 1362921351.449;
   const char* reference_names[reference_size] =
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
   const double reference_values[reference_size] =
   {
      -2.7354, -3.62058, -2.63866, -7.04316, -10.6212, -1.91041,
      -0.890352, -14.0294, -9.13237, -10.9491, -18.9445, -6.80427
   };
   const double epsilon = 0.001;

   BOOST_CHECK(result.size() == reference_size);

   for(opencdev::result_t::const_iterator it = result.begin(); it != result.end(); it++)
   {
      const string &cell_name = it->first;
      const map<opencdev::cdev_time_t, double> &values = it->second;

      BOOST_CHECK_EQUAL(values.size(), 1);

      const opencdev::cdev_time_t time = values.begin()->first;
      const double value = values.begin()->second;

      BOOST_CHECK(fabs(time - reference_time) < epsilon);

      int cell_id = -1;
      for(int i = 0; i < reference_size; i++)
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

BOOST_AUTO_TEST_SUITE_END()
