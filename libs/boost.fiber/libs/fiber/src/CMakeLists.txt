include_directories(${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB files *.cpp detail/fiber_posix.cpp detail/info_base_posix.cpp spin/*.cpp)

if(WIN32)
  add_library(boost-fiber STATIC ${files})
  set_target_properties(boost-fiber PROPERTIES PREFIX "../")
else(WIN32)
  add_library(boost-fiber SHARED ${files})
endif(WIN32)
