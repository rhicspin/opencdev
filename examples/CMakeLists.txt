# vim: set noexpandtab:

INCLUDE_DIRECTORIES(${OPENCDEV_INCLUDE_DIR})

ADD_EXECUTABLE(
	query
	query.cxx
	)

ADD_DEFINITIONS("-Wall -pedantic")

TARGET_LINK_LIBRARIES(query opencdev)
