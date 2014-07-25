# vim: set noexpandtab:

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${OPENCDEV_INCLUDE_DIR})

ADD_EXECUTABLE(
	unit_tests
	unit_tests_main.cpp
	timerange_query.cpp
	)

ADD_DEFINITIONS("-DTEST_DATA_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"")

TARGET_LINK_LIBRARIES(
	unit_tests
	${Boost_LIBRARIES}
	opencdev
	)

ADD_TEST(
	NAME timerange_query
	COMMAND $<TARGET_FILE:unit_tests> --run_test=timerange_query
	)
