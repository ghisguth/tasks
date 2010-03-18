cmake_minimum_required(VERSION 2.6)
project(tasks)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif(NOT CMAKE_BUILD_TYPE)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wunused")

set(BOOST_COMPONENTS thread)
find_package(Boost COMPONENTS ${BOOST_COMPONENTS})
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
  link_directories(${Boost_LIBRARY_DIRS})
else(Boost_FOUND)
  message( FATAL_ERROR " boost not installed" )
endif(Boost_FOUND)


subdirs(libs)
subdirs(src)
