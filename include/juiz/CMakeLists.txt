add_subdirectory(utils)

set(nk_public_headers

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
nerikiri.h 
object.h 

operation_api.h 
operation.h 

process_api.h 
process_store.h 
process.h 

systemeditor.h 

topic_api.h 
topic.h 

value.h 
)


set(nk_headers 
  ${nk_public_headers}
)


set(nk_hdrs)
set(nk_public_hdrs)

foreach(a IN ITEMS ${nk_headers})
  set(nk_hdrs ${nk_hdrs} ${CMAKE_CURRENT_SOURCE_DIR}/${a})
endforeach()

foreach(a IN ITEMS ${nk_public_headers})
  set(nk_public_hdrs ${nk_public_hdrs} include/nerikiri/${a})
endforeach()

set(nk_public_hdrs_ ${nk_public_hdrs} PARENT_SCOPE)
set(nk_hdrs_ ${nk_hdrs} ${nk_util_hdrs_} PARENT_SCOPE)

set(nk_public_util_hdrs__ ${nk_public_util_hdrs_} PARENT_SCOPE)
# set(nk_hdrs_ ${nk_hdrs} PARENT_SCOPE)