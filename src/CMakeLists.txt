set(nk_base_srcs process.cpp process_store.cpp operation.cpp 
#operationinfo.cpp 
value.cpp 
ec.cpp
# valuemap.cpp 
logger.cpp 
#broker.cpp
objectmapper.cpp 
#brokerselector.cpp 
dllproxy.cpp brokers/corebroker.cpp
#brokerinfo.cpp brokerdictionary.cpp
signal.cpp connection.cpp connectiondictionary.cpp container.cpp)
set(nk_http_srcs brokers/http/httpbroker.cpp datatype/json.cpp) 
set(nk_se_srcs systemeditor/systemeditor.cpp)
set(nk_srcs ${nk_base_srcs} ${nk_http_srcs} ${nk_se_srcs})

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/cpp-httplib)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/rapidjson/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/spdlog/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/webi/include)
set(NK_BASE_LIBS webi)

add_library(nerikiri SHARED ${nk_srcs})
target_link_libraries(nerikiri ${NK_BASE_LIBS})


set(ECS)
function(add_ec NAME)
  add_library(${NAME} SHARED ecs/${NAME}.cpp)
  target_link_libraries(${NAME} nerikiri ${EXAMPLE_LIBS})
  set(ECS ${NAME} ${ECS})
  set(ECS ${ECS} PARENT_SCOPE)
endfunction(add_ec)

set(BROKERS)
function(add_broker DIR NAME)
  add_library(${NAME} SHARED brokers/${DIR}/${NAME}.cpp)
  target_link_libraries(${NAME} nerikiri ${EXAMPLE_LIBS})
  set(BROKERS ${NAME} ${BROKERS})
  set(BROKERS ${BROKERS} PARENT_SCOPE)
endfunction(add_broker)
add_broker(http HTTPBroker)