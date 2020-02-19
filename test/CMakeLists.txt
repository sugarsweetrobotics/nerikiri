# add_compile_options(-std=gnu++17)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/Catch2/single_include/catch2)

OPTION(TEST_HTTP "Http Server Client Test" OFF)
OPTION(TEST_HTTPBROKER "HttpBroker Test" OFF)
OPTION(TEST_PROCESS "HttpBroker Test" OFF)


function(declare_test arg1)
  add_executable(${arg1}_test ${arg1}_test.cpp)
  target_link_libraries(${arg1}_test nerikiri Catch2::Catch2)
  add_test(NAME ${arg1}_test COMMAND $<TARGET_FILE:${arg1}_test>)
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

