/* vim: set sw=3: */

#include <boost/test/unit_test.hpp>

#include <opencdev.h>

BOOST_AUTO_TEST_SUITE(copy)

// A test to see if pimpl memory management works fine
BOOST_AUTO_TEST_CASE(double_free_test)
{
   opencdev::DB db(TEST_DATA_PATH);
   opencdev::result_t result;

   {
       opencdev::DB db2(db);
       db2.query_timerange("RHIC/Polarimeter/Yellow/biasReadbacks.logreq", 0, 1, &result);
       BOOST_CHECK(result.size() == 0);
   }

   db.query_timerange("RHIC/Polarimeter/Yellow/biasReadbacks.logreq", 0, 1, &result);
   BOOST_CHECK(result.size() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
