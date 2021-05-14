set(nk_public_headers

broker_api.h 
client_proxy_api.h 

connection_api.h
connection_proxy.h
connection.h

container_api.h
container.h

corebroker.h 
crud_broker_proxy_api.h 
crud_broker_proxy_base.h 
crud_broker.h 

ec_api.h
ec_factory_api.h
ec.h

factory_api.h 
functional.h
geometry.h 
json.h 
logger.h 
naming.h 
nerikiri.h 
object.h 
operation_api.h 
operation.h 
os.h 
process_api.h 
process_store.h 
process.h 
signal.h 
stringutil.h 
systemeditor.h 
topic_api.h 
topic_factory_api.h 
topic.h 
value.h 
)


set(nk_headers 
  ${nk_publish_headers}
)


set(nk_hdrs)
set(nk_public_hdrs)

foreach(a IN ITEMS ${nk_headers})
  set(nk_hdrs ${nk_hdrs} include/nerikiri/${a})
endforeach()

foreach(a IN ITEMS ${nk_public_headers})
  set(nk_public_hdrs ${nk_public_hdrs} include/nerikiri/${a})
endforeach()

set(nk_public_hdrs_ ${nk_public_hdrs} PARENT_SCOPE)
set(nk_hdrs_ ${nk_hdrs} PARENT_SCOPE)