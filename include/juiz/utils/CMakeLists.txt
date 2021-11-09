set(juiz_public_util_headers
  dllproxy.h
  functional.h
  json.h 
  naming.h 
  os.h 
  signal.h 
  stringutil.h 
  yaml.h
  argparse.h
)


set(juiz_util_headers 
  ${juiz_public_util_headers}
)


set(juiz_util_hdrs)
set(juiz_public_util_hdrs)

foreach(a IN ITEMS ${juiz_util_headers})
  set(juiz_util_hdrs ${juiz_util_hdrs} ${CMAKE_CURRENT_SOURCE_DIR}/${a})
endforeach()

foreach(a IN ITEMS ${juiz_public_util_headers})
  set(juiz_public_util_hdrs ${juiz_public_util_hdrs} include/juiz/utils/${a})
endforeach()

set(juiz_public_util_hdrs_ ${juiz_public_util_hdrs} PARENT_SCOPE)
set(juiz_util_hdrs_ ${juiz_util_hdrs} PARENT_SCOPE)