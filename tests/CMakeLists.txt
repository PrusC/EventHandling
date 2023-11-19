
cmake_minimum_required(VERSION 3.16)
project(Tests VERSION 0.1.0 LANGUAGES CXX)

#include_directories(${GTEST_INCLUDE_DIRS})

set(LINK_PUBLIC_LIBS
  ${GTEST_BOTH_LIBRARIES}
  #${ADD_PTHREAD_LIB}
  #${CMAKE_DL_LIBS}
)

set(INCLUDE_DIRS
  ${GTEST_INCLUDE_DIRS}
  ${EH_INCLUDE_DIR}
)

set(TEST_FILES
  test_invocable
  test_argument_pack
  test_wrapped_call
  test_delegate
  test_internal_executor
  test_delegate_executor
  test_events
)

foreach (file ${TEST_FILES})
  add_executable(${file} ${file}.cpp)

  target_include_directories(${file}
  PUBLIC 
    ${INCLUDE_DIRS}
  )

  set_target_properties(${file} PROPERTIES POSITION_INDEPENDENT_CODE ON)

  target_link_libraries(${file} 
    PUBLIC
      ${LINK_PUBLIC_LIBS} 
      EventHandling 
  )

  gtest_discover_tests(${file})

  install(TARGETS ${file} DESTINATION ${CMAKE_INSTALL_PREFIX})

endforeach()
