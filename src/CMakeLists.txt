include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(tasks main.cpp)
target_link_libraries(tasks m ${Boost_LIBRARIES} boost-fiber boost-task)
