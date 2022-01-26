
cmake_minimum_required(VERSION 3.16)
project(EventHandling VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(HEADER_DIR EventHandling)

set (header_files
	${HEADER_DIR}/delegate.hpp
	${HEADER_DIR}/multicastdelegate.hpp
	${HEADER_DIR}/eventhandler.hpp
	${HEADER_DIR}/event.hpp
	${HEADER_DIR}/delegate.hpp
)

add_library(EventHandling INTERFACE ${header_files})

if(BUILD_UNIT_TESTS)
  add_subdirectory(tests)
	enable_testing()
	include(GoogleTest)
endif()