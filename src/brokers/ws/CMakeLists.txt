

add_subdirectory(IXWebSocket)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/IXWebSocket)

message(${CMAKE_CURRENT_SOURCE_DIR})
set(BROKERS)

find_package(ZLIB)

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

add_broker(WSBroker
  SRCS ws_broker.cpp ws_broker_proxy.cpp ws_client_ixwebsocket.cpp ws_server_ixwebsocket.cpp
  LIBS ixwebsocket 
)

install(TARGETS WSBroker DESTINATION share/juiz/brokers)