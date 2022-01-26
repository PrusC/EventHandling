
cmake_minimum_required(VERSION 3.16)
project(Tests VERSION 0.1.0 LANGUAGES CXX)

#set(INCLUDED_PARENT_PATH "../")
#add_library(EventHandling INTERFACE "../")
include_directories("../")

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(tests tests.cpp)

set_target_properties(tests PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(tests PUBLIC
  ${GTEST_LIBRARY}
  ${GTEST_MAIN_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT}
  INTERFACE EventHandling 
)