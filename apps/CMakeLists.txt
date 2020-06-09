include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(Threads REQUIRED)

function(add_app NAME)
  add_executable(${NAME} ${NAME}.cpp)
  target_link_libraries(${NAME} nerikiri Threads::Threads)

  set_target_properties(${NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  )

  set_target_properties(${NAME} PROPERTIES PREFIX "")
endfunction(add_app)


add_app(nkd)


set(APPS nkd)

install(TARGETS ${APPS}
)