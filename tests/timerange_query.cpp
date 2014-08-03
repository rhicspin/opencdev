/* vim: set sw=3: */

#include <string>
#include <map>
#include <math.h>

#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <opencdev.h>

using namespace std;

/*
 * Obtained by running
 * exportLoggerData \
 *  -logger 'RHIC/Polarimeter/Yellow/biasReadbacks' \
 *  -timeformat 'unix' -showmissingdatawith x \
 *  -start 'Sun Mar 10 09:15:00 2013' -stop 'Sun Mar 10 09:16:00 2013'
 *
 *  Note that exportLoggerData accepts NY time, while we work in UTC
 */
const int reference_column_count = 12;
const double reference_time = 1362921351.449;
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
   -2.7354, -3.62058, -2.63866, -7.04316, -10.6212, -1.91041,
   -0.890352, -14.0294, -9.13237, -10.9491, -18.9445, -6.80427
};

int find_cell(const string &cell_name)
{
   int cell_id = -1;
   for(int i = 0; i < reference_column_count; i++)
   {
      if (cell_name == reference_names[i])
      {
         cell_id = i;
         break;
      }
   }
   return cell_id;
}

const double starttime = 1362921300;
const double endtime = 1362921360;

const double epsilon = 0.001;


BOOST_AUTO_TEST_SUITE(timerange_query)

BOOST_AUTO_TEST_CASE(check_load_fail)
{
   BOOST_CHECK_THROW(opencdev::LocalLogReader log_reader("/this/does/not/exist"), const char*);
}

BOOST_AUTO_TEST_CASE(selfcheck_timestamps)
{
   using namespace boost::posix_time;
   using namespace boost::gregorian;

   ptime t1(date(2013, Mar, 10), time_duration(13, 15, 0));
   ptime t2(date(2013, Mar, 10), time_duration(13, 16, 0));
   const double ticks_per_second =
      time_duration::rep_type::ticks_per_second;
   ptime unix_epoch(boost::gregorian::date(1970,1,1));
   double t1v = (t1 - unix_epoch).ticks() / ticks_per_second;
   double t2v = (t2 - unix_epoch).ticks() / ticks_per_second;

   BOOST_CHECK_EQUAL(t1v, starttime);
   BOOST_CHECK_EQUAL(t2v, endtime);
}

BOOST_AUTO_TEST_CASE(check_query)
{
   opencdev::LocalLogReader log_reader(TEST_DATA_PATH);
   opencdev::result_t result;

   log_reader.query_timerange("RHIC/Polarimeter/Yellow/biasReadbacks.logreq", starttime, endtime, &result);

   BOOST_CHECK_EQUAL(result.size(), reference_column_count);

   for(opencdev::result_t::const_iterator it = result.begin(); it != result.end(); it++)
   {
      const string &cell_name = it->first;
      const map<opencdev::cdev_time_t, double> &values = it->second;

      BOOST_CHECK_EQUAL(values.size(), 1);

      const opencdev::cdev_time_t time = values.begin()->first;
      const double value = values.begin()->second;

      BOOST_CHECK(fabs(time - reference_time) < epsilon);

      int cell_id = find_cell(cell_name);
      BOOST_CHECK(cell_id >= 0);
      BOOST_CHECK(fabs(value - reference_values[cell_id]) < epsilon);
   }
}

BOOST_AUTO_TEST_CASE(check_query_mean)
{
   opencdev::LocalLogReader log_reader(TEST_DATA_PATH);
   opencdev::mean_result_t result;

   log_reader.query_timerange_mean("RHIC/Polarimeter/Yellow/biasReadbacks.logreq", starttime, endtime, &result);

   BOOST_CHECK_EQUAL(result.size(), reference_column_count);

   for(opencdev::mean_result_t::const_iterator it = result.begin(); it != result.end(); it++)
   {
      const string &cell_name = it->first;
      double value = it->second;

      int cell_id = find_cell(cell_name);
      BOOST_CHECK(cell_id >= 0);
      BOOST_CHECK(fabs(value - reference_values[cell_id]) < epsilon);
   }
}

BOOST_AUTO_TEST_SUITE_END()
