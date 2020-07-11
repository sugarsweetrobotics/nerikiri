# add_compile_options(-std=gnu++17)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/Catch2/single_include/catch2)

OPTION(TEST_HTTP "Http Server Client Test" OFF)
OPTION(TEST_HTTPBROKER "HttpBroker Test" OFF)
OPTION(TEST_OPTION "Option Parser Test" OFF)
OPTION(TEST_PROCESS "Process Test" OFF)
OPTION(TEST_OPERATION "Operation Test" OFF)
OPTION(TEST_CONTAINER "Container Test" OFF)
OPTION(TEST_EC "ExecutionContext Test" OFF)
OPTION(TEST_FSM "FSM Test" ON)


set(EXAMPLE_LIBS nerikiri)

function(declare_test arg1)
  add_executable(${arg1}_test ${arg1}_test.cpp)
  target_link_libraries(${arg1}_test nerikiri Catch2::Catch2)
  add_dependencies(${arg1}_test nerikiri ${EXAMPLE_MODULES})
  add_test(NAME ${arg1}_test COMMAND $<TARGET_FILE:${arg1}_test>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test
  )
endfunction()

if (${TEST_HTTP})
  declare_test(http)
endif ()

if (${TEST_HTTPBROKER})
  declare_test(httpbroker)
endif ()


if (${TEST_PROCESS})
  declare_test(process)
endif ()

if (${TEST_OPERATION})
  declare_test(operation)
endif ()

if (${TEST_CONTAINER})
  declare_test(container)
endif ()

if (${TEST_EC})
  declare_test(ec)
endif ()

if (${TEST_FSM})
  declare_test(fsm)
endif ()


if (${TEST_OPTION})
  declare_test(option)
endif ()