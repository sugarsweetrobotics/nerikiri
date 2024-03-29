set(juiz_base_srcs 



anchor/anchor_container.h
anchor/anchor_container.cpp
anchor/StaticPeriodicAnchor.cpp
anchor/DynamicPeriodicAnchor.cpp

pose/static_transformation_operation.cpp
pose/dynamic_transformation_operation.cpp

# broker/brokerfactory.cpp
broker/broker_api.cpp
broker/broker_proxy_api.cpp
broker/broker_factory_api.cpp
broker/crud_broker.cpp
broker/crud_broker_base.cpp
broker/crud_broker_proxy_base.cpp
broker/core_broker.cpp
broker/object_mapper.cpp 

container/container_api.cpp
container/container_base.cpp
container/container_base.h
container/container_factory_base.cpp
container/container_factory_base.h
container/container_operation_base.cpp
container/container_operation_factory_api.cpp
container/container_operation_factory_base.cpp
container/container_operation_factory_base.h
container/container.cpp 
container/container_proxy.cpp 
container/container_proxy.h
container/model_data.cpp
# container/container_factory_api.cpp


connection/connection_proxy.cpp
connection/connection_builder.cpp
connection/connection.cpp 

dllproxy.cpp 

# ec/ec.cpp
ec/ec_api.cpp
ec/ec_factory_api.cpp

ec/ec_container.h
ec/ec_container.cpp

fsm/fsm_container.cpp
#new_fsm/fsm.cpp
# new_fsm/fsm_api.cpp
#new_fsm/fsm_state.cpp
# new_fsm/fsm_state_api.cpp
# new_fsm/fsm_factory_api.cpp

#fsm/fsm.cpp
#fsm/fsm_state.cpp
#fsm/fsm_api.cpp
#fsm/fsm_state_api.cpp
#fsm/fsm_proxy.cpp
#fsm/fsm_factory_api.cpp

utils/json.cpp 
utils/base64.c
utils/yaml.cpp
logger.cpp 
naming.cpp

os.cpp
operation/operation.cpp 
operation/operation_base.cpp
operation/operation_api.cpp
# operation/operation_factory_api.cpp
operation/operation_factory.cpp
operation/operation_outlet_api.cpp
operation/operation_inlet_api.cpp
operation/operation_with_argument.cpp
operation/operation_proxy.cpp
operation/operation_inlet_base.cpp
operation/operation_outlet_base.cpp


object.cpp
factory_api.cpp
objectfactory.cpp

proxy_builder.cpp
process/process_impl.h
process/process_impl.cpp 
process/process_store.cpp 
process/processconfigparser.cpp
process/process_builder.cpp


process/moduleloader.cpp
#operationinfo.cpp 

signal.cpp 

value.cpp 

topic/topic_api.cpp
topic/topic_factory.cpp
#broker.cpp
#brokerinfo.cpp brokerdictionary.cpp
)
set(juiz_http_srcs brokers/http/httpbroker.cpp ) 
set(juiz_se_srcs systemeditor/systemeditor.cpp)
set(juiz_srcs ${juiz_base_srcs} )

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/cpp-httplib)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/rapidjson/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/spdlog/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/webi/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/yaml-cpp/include)
include_directories(${CMAKE_BINARY_DIR}/thirdparty/yaml-cpp/include)
set(NK_BASE_LIBS yaml-cpp)

add_library(juiz SHARED ${juiz_srcs} ${juiz_headers})
target_link_libraries(juiz ${NK_BASE_LIBS})
set_target_properties(juiz
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)


if (FALSE)
set(ECS)
function(add_ec NAME)
  add_library(${NAME} SHARED ecs/${NAME}.cpp)
  target_link_libraries(${NAME} juiz ${EXAMPLE_LIBS})
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
  target_link_libraries(${NAME} juiz ${EXAMPLE_LIBS})
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
