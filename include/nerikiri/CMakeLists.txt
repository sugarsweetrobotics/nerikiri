set(nk_public_headers
argparse.h
abstractbrokerproxy.h
base64.h 
broker.h 
brokerapi.h 
connection.h
connectionbuilder.h 
connectiondictionary.h 
container.h
containerbase.h 
containerfactory.h
containeroperation.h
containeroperationfactory.h
containeroperationfactorybase.h
corebroker.h 
dllproxy.h 
ec.h 
fsm.h 
functional.h 
json.h 
logger.h 
moduleloader.h 
naming.h 
nerikiri.h
nullbroker.h 
nullbrokerproxy.h 
object.h 
objectfactory.h 
objectmapper.h 
operation.h 
operation_base.h 
operationfactory.h 
os.h 
process.h 
process_store.h 
processconfigparser.h 
signal.h 
stringutil.h 
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