set(nk_base_srcs process.cpp operation.cpp operationinfo.cpp state.cpp statemap.cpp logger.cpp brokerinfo.cpp brokerdictionary.cpp signal.cpp)
set(nk_http_srcs http/httpbroker.cpp http/server.cpp http/client.cpp)
set(nk_srcs ${nk_base_srcs} ${nk_http_srcs})

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/cpp-httplib)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/spdlog/include)

set(NK_BASE_LIBS)

add_library(nerikiri ${nk_srcs})
target_link_libraries(nerikiri ${NK_BASE_LIBS})


