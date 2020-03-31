set(nk_base_srcs process.cpp process_store.cpp operation.cpp operationinfo.cpp value.cpp valuemap.cpp logger.cpp broker.cpp
objectmapper.cpp brokerselector.cpp
#brokerinfo.cpp brokerdictionary.cpp
signal.cpp connection.cpp connectiondictionary.cpp container.cpp)
set(nk_http_srcs http/httpbroker.cpp datatype/json.cpp) # http/server.cpp http/client.cpp)
set(nk_se_srcs systemeditor/systemeditor.cpp)
set(nk_srcs ${nk_base_srcs} ${nk_http_srcs} ${nk_se_srcs})

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/cpp-httplib)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/rapidjson/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/spdlog/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/webi/include)
set(NK_BASE_LIBS webi)

add_library(nerikiri ${nk_srcs})
target_link_libraries(nerikiri ${NK_BASE_LIBS})


