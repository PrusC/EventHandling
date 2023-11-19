
cmake_minimum_required(VERSION 3.16)
project(EventHandling VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(EH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(EH_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/EventHandling)
set(EH_HEADERS_DIR EventHandling)

set (HEADER_FILES
  ${EH_HEADERS_DIR}/utils/ptr.hpp
  ${EH_HEADERS_DIR}/utils/invocable_element.hpp
  ${EH_HEADERS_DIR}/utils/helper.hpp
  ${EH_HEADERS_DIR}/utils/argument_pack.hpp
  ${EH_HEADERS_DIR}/utils/result_store.hpp
  ${EH_HEADERS_DIR}/utils/wrapped_call.hpp
  ${EH_HEADERS_DIR}/utils/callback_queue_base.hpp
  ${EH_HEADERS_DIR}/utils/callback_queue.hpp
  ${EH_HEADERS_DIR}/delegate_invoke_type.h
  ${EH_HEADERS_DIR}/delegate_base.h
  ${EH_HEADERS_DIR}/delegate.hpp
  ${EH_HEADERS_DIR}/delegate_internal_executor.hpp
  ${EH_HEADERS_DIR}/delegate_executor.hpp
  ${EH_HEADERS_DIR}/event_handler.hpp
  ${EH_HEADERS_DIR}/multicast_delegate.hpp
  ${EH_HEADERS_DIR}/thread.hpp
  ${EH_HEADERS_DIR}/task.hpp
  ${EH_HEADERS_DIR}/event_system.h
)

set (SOURCE_FILES
  ${EH_SOURCE_DIR}/thread.cpp
  ${EH_HEADERS_DIR}/task.cpp
  ${EH_HEADERS_DIR}/event_system.cpp
)

add_library(EventHandling 
  STATIC 
    ${HEADER_FILES}
	${SOURCE_FILES}
)


if(BUILD_MAIN_EXECUTABLE)
	add_executable(main main.cpp)
	target_link_libraries(main PUBLIC EventHandling)
endif()

if(BUILD_UNIT_TESTS)
  find_package(GTest REQUIRED)

  add_subdirectory(tests)

  enable_testing()
  include(GoogleTest)
endif()

foreach ( file ${HEADER_FILES} )
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/include/${dir})
endforeach()

install(TARGETS EventHandling LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)