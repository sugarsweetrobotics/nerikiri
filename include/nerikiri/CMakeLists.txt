set(nk_headers 
  nerikiri.h
  util/argparse.h
  util/dllproxy.h
  util/os.h
  util/functional.h
  util/logger.h
  util/naming.h
  util/signal.h
  operationfactory.h
#  brokers/
  ec.h
  process.h
  connection.h
  process_store.h
  connectiondictionary.h
  processconfigparser.h
  containers/container.h
  containers/containerfactory.h
  object.h
  systemeditor.h
  containers/containeroperation.h
  objectfactory.h
  timerec.h
  containers/containeroperationfactory.h
  objectmapper.h
  value.h
  containers/containeroperationfactorybase.h
  operation.h
#  datatype/
  operationbase.h

)


set(nk_hdrs)

foreach(a IN ITEMS ${nk_headers})
  set(nk_hdrs ${nk_hdrs} ${PROJECT_SOURCE_DIR}/include/nerikiri/${a})
endforeach()


message(${nk_hdrs})

set(nk_hdrs_ ${nk_hdrs} PARENT_SCOPE)