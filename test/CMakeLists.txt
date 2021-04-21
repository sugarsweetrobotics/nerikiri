# add_compile_options(-std=gnu++17)

if (BUILD_NK_TEST)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/Catch2/single_include/catch2)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/Catch2/src)

OPTION(BUILD_NK_TEST_HTTP "Http Server Client Test" OFF)
OPTION(BUILD_NK_TEST_HTTPBROKER "HttpBroker Test" OFF)
OPTION(BUILD_NK_TEST_OPTION "Option Parser Test" OFF)
OPTION(BUILD_NK_TEST_TOPIC "Topic Test" ON)
OPTION(BUILD_NK_TEST_PROCESS "Process Test" OFF)
OPTION(BUILD_NK_TEST_OPERATION "Operation Test" OFF)
OPTION(BUILD_NK_TEST_CONTAINER "Container Test" OFF)
OPTION(BUILD_NK_TEST_EC "ExecutionContext Test" OFF)
OPTION(BUILD_NK_TEST_FSM "FSM Test" ON)
OPTION(BUILD_NK_TEST_CONNECTION "Connection Test" ON)
OPTION(BUILD_NK_TEST_HTTPBROKER "HTTP Broker Test" ON)
OPTION(BUILD_NK_TEST_GEOMETRY "Geometry Test" ON)

set(EXAMPLE_LIBS nerikiri)

function(declare_test arg1)
  add_executable(${arg1}_test ${arg1}_test.cpp)
  target_link_libraries(${arg1}_test nerikiri Catch2::Catch2WithMain)
  add_dependencies(${arg1}_test nerikiri ${EXAMPLE_MODULES})
  add_test(NAME ${arg1}_test COMMAND $<TARGET_FILE:${arg1}_test>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test
  )
endfunction()

if (${BUILD_NK_TEST_HTTP})
  declare_test(http)
endif ()


if (${BUILD_NK_TEST_TOPIC})
  declare_test(topic)
endif ()

if (${BUILD_NK_TEST_GEOMETRY})
  declare_test(geometry)
endif ()

if (${BUILD_NK_TEST_HTTPBROKER})
  declare_test(httpbroker)
endif ()


if (${BUILD_NK_TEST_PROCESS})
  declare_test(process)
endif ()

if (${BUILD_NK_TEST_OPERATION})
  declare_test(operation)
endif ()

if (${BUILD_NK_TEST_CONTAINER})
  declare_test(container)
endif ()

if (${BUILD_NK_TEST_EC})
  declare_test(ec)
endif ()

if (${BUILD_NK_TEST_FSM})
  declare_test(fsm)
endif ()

if (${BUILD_NK_TEST_CONNECTION})
  declare_test(connection)
endif ()

if (${BUILD_NK_TEST_OPTION})
  declare_test(option)
endif ()


endif (BUILD_NK_TEST)
