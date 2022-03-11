add_subdirectory(utils)

set(juiz_public_headers

broker_api.h 
client_proxy_api.h 

connection_api.h
connection_proxy.h
connection.h

container_api.h
container.h

crud_broker_proxy_api.h 
crud_broker_proxy_base.h 
crud_broker.h 

ec_api.h
ec.h

factory_api.h 

geometry.h 
logger.h 
juiz.h 
object.h 

model_data.h

operation_api.h 
operation.h 

process_api.h 
process_store.h 
process.h 

path_parser.h

systemeditor.h 

topic_api.h 
topic.h 

value.h 
)


set(juiz_headers 
  ${juiz_public_headers}
)


set(juiz_hdrs)
set(juiz_public_hdrs)

foreach(a IN ITEMS ${juiz_headers})
  set(juiz_hdrs ${juiz_hdrs} ${CMAKE_CURRENT_SOURCE_DIR}/${a})
endforeach()

foreach(a IN ITEMS ${juiz_public_headers})
  set(juiz_public_hdrs ${juiz_public_hdrs} include/juiz/${a})
endforeach()

set(juiz_public_hdrs_ ${juiz_public_hdrs} PARENT_SCOPE)
set(juiz_hdrs_ ${juiz_hdrs} ${juiz_util_hdrs_} PARENT_SCOPE)

set(juiz_public_util_hdrs__ ${juiz_public_util_hdrs_} PARENT_SCOPE)
# set(juiz_hdrs_ ${juiz_hdrs} PARENT_SCOPE)