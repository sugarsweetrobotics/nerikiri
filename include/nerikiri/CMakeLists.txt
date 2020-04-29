set(nk_headers 
  nerikiri.h
  argparse.h
  dllproxy.h
  operationfactory.h
#  brokers/
  ec.h
  os.h
  functional.h
  process.h
  connection.h
  logger.h
  process_store.h
  connectiondictionary.h
  naming.h
  processconfigparser.h
  container.h
  nerikiri.h
  signal.h
  containerfactory.h
  object.h
  systemeditor.h
  containeroperation.h
  objectfactory.h
  timerec.h
  containeroperationfactory.h
  objectmapper.h
  value.h
  containeroperationfactorybase.h
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