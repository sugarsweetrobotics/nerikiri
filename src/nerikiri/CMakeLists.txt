set(nk_base_srcs 

base64.c
brokerfactory.cpp
broker_proxy_api.cpp
broker.cpp
container_factory_base.cpp
connection_proxy.cpp
container_api.cpp
connection_builder.cpp
connection.cpp 
connectiondictionary.cpp 
container.cpp 
crud_broker.cpp
crud_broker_base.cpp
corebroker.cpp
dllproxy.cpp 

ec.cpp
fsm.cpp

json.cpp 
logger.cpp 
moduleloader.cpp
naming.cpp

operation.cpp 
os.cpp
operation_base.cpp
operation_api.cpp
operation_outlet_api.cpp
operation_inlet_api.cpp
object.cpp
object_mapper.cpp 
objectfactory.cpp
operation_proxy.cpp

proxy_builder.cpp
process.cpp 
process_store.cpp 
processconfigparser.cpp
process_builder.cpp
#operationinfo.cpp 

signal.cpp 

value.cpp 
#broker.cpp
#brokerinfo.cpp brokerdictionary.cpp
)
set(nk_http_srcs brokers/http/httpbroker.cpp ) 
set(nk_se_srcs systemeditor/systemeditor.cpp)
set(nk_srcs ${nk_base_srcs} )

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/cpp-httplib)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/rapidjson/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/spdlog/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/webi/include)
set(NK_BASE_LIBS )

add_library(nerikiri SHARED ${nk_srcs} ${nk_headers})
target_link_libraries(nerikiri ${NK_BASE_LIBS})
set_target_properties(nerikiri
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)


if (FALSE)
set(ECS)
function(add_ec NAME)
  add_library(${NAME} SHARED ecs/${NAME}.cpp)
  target_link_libraries(${NAME} nerikiri ${EXAMPLE_LIBS})
  set_target_properties(${NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" 
  )

  set_target_properties(${NAME} PROPERTIES PREFIX "")
  set(ECS ${NAME} ${ECS})
  set(ECS ${ECS} PARENT_SCOPE)
endfunction(add_ec)


add_ec(TimerEC)
add_ec(OneShotEC)

set(BROKERS)
function(add_broker DIR NAME)
  add_library(${NAME} SHARED brokers/${DIR}/${NAME}.cpp)
  target_link_libraries(${NAME} nerikiri ${EXAMPLE_LIBS})
  set_target_properties(${NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" 
  )

  set_target_properties(${NAME} PROPERTIES PREFIX "")
  set(BROKERS ${NAME} ${BROKERS})
  set(BROKERS ${BROKERS} PARENT_SCOPE)
endfunction(add_broker)


add_broker(http HTTPBroker)

endif (FALSE)
