cmake_minimum_required(VERSION 2.6)
project(tasks)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif(NOT CMAKE_BUILD_TYPE)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wunused")

set(BOOST_COMPONENTS thread system)
find_package(Boost 1.41 COMPONENTS ${BOOST_COMPONENTS})
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
  link_directories(${Boost_LIBRARY_DIRS})
else(Boost_FOUND)
  message( FATAL_ERROR " boost 1.41 not installed" )
endif(Boost_FOUND)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/boost.move/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/boost.atomic/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/boost.fiber/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/boost.task/)


subdirs(libs)
subdirs(src)
