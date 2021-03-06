# vim: set noexpandtab:

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${OPENCDEV_INCLUDE_DIR})

ADD_EXECUTABLE(
	unit_tests
	unit_tests_main.cpp
	copy.cpp
	fill_query.cpp
	sdds.cpp
	timerange_query.cpp
	)

ADD_DEFINITIONS("-DTEST_DATA_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"")

TARGET_LINK_LIBRARIES(
	unit_tests
	${Boost_LIBRARIES}
	opencdev
	)

ADD_TEST(
	NAME copy
	COMMAND $<TARGET_FILE:unit_tests> --run_test=copy
	)
ADD_TEST(
	NAME fill_query
	COMMAND $<TARGET_FILE:unit_tests> --run_test=fill_query
	)
ADD_TEST(
	NAME sdds
	COMMAND $<TARGET_FILE:unit_tests> --run_test=sdds
	)
ADD_TEST(
	NAME timerange_query
	COMMAND $<TARGET_FILE:unit_tests> --run_test=timerange_query
	)
