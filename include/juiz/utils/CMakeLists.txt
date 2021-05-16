set(nk_public_util_headers
  dllproxy.h
  functional.h
  json.h 
  naming.h 
  os.h 
  signal.h 
  stringutil.h 
)


set(nk_util_headers 
  ${nk_public_util_headers}
)


set(nk_util_hdrs)
set(nk_public_util_hdrs)

foreach(a IN ITEMS ${nk_util_headers})
  set(nk_util_hdrs ${nk_util_hdrs} ${CMAKE_CURRENT_SOURCE_DIR}/${a})
endforeach()

foreach(a IN ITEMS ${nk_public_util_headers})
  set(nk_public_util_hdrs ${nk_public_util_hdrs} include/juiz/utils/${a})
endforeach()

set(nk_public_util_hdrs_ ${nk_public_util_hdrs} PARENT_SCOPE)
set(nk_util_hdrs_ ${nk_util_hdrs} PARENT_SCOPE)