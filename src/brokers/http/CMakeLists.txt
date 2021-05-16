

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/webi/include)

message(${CMAKE_CURRENT_SOURCE_DIR})
set(BROKERS)

function(add_broker NAME)
  cmake_parse_arguments(ADD_BROKER "" "" "SRCS;LIBS" ${ARGN})
  add_library(${NAME} SHARED ${ADD_BROKER_SRCS})
  message(${ADD_BROKER_SRCS})
  target_link_libraries(${NAME} juiz ${ADD_BROKER_LIBS})
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


add_broker(HTTPBroker
  SRCS http_broker.cpp http_broker_proxy.cpp http_client_cpphttplib.cpp http_server_cpphttplib.cpp
)

install(TARGETS HTTPBroker DESTINATION share/juiz/brokers)