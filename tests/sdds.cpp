/* vim: set sw=3: */

#include <boost/test/unit_test.hpp>

#include <opencdev.h>

using namespace std;

BOOST_AUTO_TEST_SUITE(sdds)

BOOST_AUTO_TEST_CASE(check_gzip_load)
{
   opencdev::LocalLogReader log_reader(TEST_DATA_PATH);
   opencdev::result_t result;

   // Reads 17201/RHIC/Polarimeter/Yellow/biasReadbacks/2013-03-09.2301.30.sdds
   log_reader.query_fill("RHIC/Polarimeter/Yellow/biasReadbacks", 17201, &result);

   BOOST_CHECK_EQUAL(result.size(), 12);
}

BOOST_AUTO_TEST_CASE(check_ascii)
{
   opencdev::LocalLogReader log_reader(TEST_DATA_PATH);
   opencdev::result_t result;

   // Reads 17602/RHIC/Polarimeter/Yellow/biasReadbacks/2013-07-02.1825.15.sdds
   log_reader.query_timerange("RHIC/Polarimeter/Yellow/biasReadbacks", 1375347602, 1375354830, &result);

   BOOST_CHECK_EQUAL(result.size(), 12);
}

BOOST_AUTO_TEST_SUITE_END()
