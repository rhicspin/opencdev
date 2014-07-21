# vim: set noexpandtab:

CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)
PROJECT(opencdev)

EXECUTE_PROCESS(
	COMMAND git submodule init
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)
EXECUTE_PROCESS(
	COMMAND git submodule update
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

# workaround for
# http://stackoverflow.com/questions/9948375/cmake-find-package-succeeds-but-returns-wrong-path
SET(Boost_NO_BOOST_CMAKE ON)

fIND_PACKAGE(Sqlite3)
FIND_PACKAGE(Boost 1.41 REQUIRED filesystem)

ADD_SUBDIRECTORY(contrib)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(examples)
