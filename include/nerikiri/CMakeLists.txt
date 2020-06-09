set(nk_public_headers
  nerikiri.h
  functional.h
  naming.h
  object.h
  value.h
  brokerapi.h
  connection.h
  operationbase.h
  operation.h
  operationfactory.h
  container.h
  containerfactory.h
  containeroperation.h
  containeroperationfactorybase.h
  containeroperationfactory.h
  argparse.h
  dllproxy.h
  os.h
  logger.h
  signal.h
  ec.h
  process.h
  process_store.h
  connectiondictionary.h
  processconfigparser.h
  systemeditor.h
  objectfactory.h
  timerec.h
  objectmapper.h
  topic.h
)


set(nk_headers 
  ${nk_publish_headers}
)


set(nk_hdrs)
set(nk_public_hdrs)

foreach(a IN ITEMS ${nk_headers})
  set(nk_hdrs ${nk_hdrs} ${PROJECT_SOURCE_DIR}/include/nerikiri/${a})
endforeach()

foreach(a IN ITEMS ${nk_public_headers})
  set(nk_public_hdrs ${nk_public_hdrs} ${PROJECT_SOURCE_DIR}/include/nerikiri/${a})
endforeach()

set(nk_public_hdrs_ ${nk_public_hdrs} PARENT_SCOPE)
set(nk_hdrs_ ${nk_hdrs} PARENT_SCOPE)