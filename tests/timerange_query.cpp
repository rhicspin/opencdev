/* vim: set sw=3: */

#include <boost/test/unit_test.hpp>
#include "boost/date_time/posix_time/posix_time_types.hpp"

#include <opencdev.h>

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

   ptime t1(date(2013, Mar, 10), time_duration(9, 10, 0));
   ptime t2(date(2013, Mar, 10), time_duration(9, 16, 0));
   const double ticks_per_second =
      time_duration::rep_type::ticks_per_second;
   ptime unix_epoch(boost::gregorian::date(1970,1,1));
   double t1v = (t1 - unix_epoch).ticks() / ticks_per_second;
   double t2v = (t2 - unix_epoch).ticks() / ticks_per_second;

   db.query_timerange("RHIC/Polarimeter/Yellow/biasReadbacks.logreq", t1v, t2v, &result);

   BOOST_CHECK(result.size() > 0);

   for(opencdev::result_t::const_iterator it = result.begin(); it != result.end(); it++)
   {
      const string &cell_name = it->first;
      const map<opencdev::cdev_time_t, double> &values = it->second;

      BOOST_CHECK_EQUAL(values.size(), 1);
   }
}

BOOST_AUTO_TEST_SUITE_END()
