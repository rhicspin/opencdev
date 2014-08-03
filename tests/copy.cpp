/* vim: set sw=3: */

#include <boost/test/unit_test.hpp>

#include <opencdev.h>

BOOST_AUTO_TEST_SUITE(copy)

// A test to see if pimpl memory management works fine
BOOST_AUTO_TEST_CASE(double_free_test)
{
   opencdev::LocalLogReader log_reader(TEST_DATA_PATH);
   opencdev::result_t result;

   {
       opencdev::LocalLogReader log_reader2(log_reader);
       log_reader2.query_timerange("RHIC/Polarimeter/Yellow/biasReadbacks.logreq", 0, 1, &result);
       BOOST_CHECK(result.size() == 0);
   }

   log_reader.query_timerange("RHIC/Polarimeter/Yellow/biasReadbacks.logreq", 0, 1, &result);
   BOOST_CHECK(result.size() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
