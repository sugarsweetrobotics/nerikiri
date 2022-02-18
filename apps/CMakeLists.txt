include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(Threads REQUIRED)

function(add_app NAME)
  add_executable(${NAME} ${NAME}.cpp)
  target_link_libraries(${NAME} juiz Threads::Threads)

  set_target_properties(${NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  )

  set_target_properties(${NAME} PROPERTIES PREFIX "")
endfunction(add_app)


add_app(juizd)

add_app(juiz_create_component)

set(APPS juizd juiz_create_component)

install(TARGETS ${APPS} )
