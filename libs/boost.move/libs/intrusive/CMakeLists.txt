#----------------------------------------------------------------------------
# This file was automatically generated from the original CMakeLists.txt file
# Add a variable to hold the headers for the library
set (lib_headers
    intrusive_ptr.hpp
    intrusive
)

# Add a library target to the build system
boost_library_project(
  intrusive
  #  SRCDIRS 
  #  TESTDIRS 
  HEADERS ${lib_headers}
  #  DOCDIRS 
  #  DESCRIPTION 
  MODULARIZED 
  #  AUTHORS 
  #  MAINTAINERS 
)


