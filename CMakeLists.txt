
cmake_minimum_required(VERSION 3.20)
project(EventHandling VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_subdirectory(EventHandling)
include_directories(EventHandling)

add_executable("main" main.cpp)

target_include_directories(EventHandling INTERFACE)
target_link_libraries("main" INTERFACE EventHandling)