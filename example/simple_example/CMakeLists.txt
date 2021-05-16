include_directories(${CMAKE_SOURCE_DIR}/include)

set(OPERATIONS)
set(EXT_LIBS)

function(add_operation NAME)
  add_library(${NAME} SHARED ${NAME}.cpp)
  add_dependencies(${NAME} juiz)
  target_link_libraries(${NAME} juiz ${EXT_LIBS})
  set_target_properties(${NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  )
  set_target_properties(${NAME} PROPERTIES PREFIX "")

  set(OPERATIONS ${NAME} ${OPERATIONS})
  set(OPERATIONS ${OPERATIONS} PARENT_SCOPE)
endfunction(add_operation)

set(CONTAINERS)
function(add_container NAME)
  add_library(${NAME} SHARED ${NAME}.cpp)
  add_dependencies(${NAME} juiz)
  target_link_libraries(${NAME} juiz ${EXT_LIBS})

  set_target_properties(${NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  )

  set_target_properties(${NAME} PROPERTIES PREFIX "")
  set(CONTAINERS ${NAME} ${CONTAINERS})
  set(CONTAINERS ${CONTAINERS} PARENT_SCOPE)
endfunction(add_container)

if (BUILD_SIMPLE_EXAMPLE)
add_operation(one)
add_operation(increment)
add_operation(decrement)
add_operation(add)
add_operation(zero)

add_container(MyStruct)
add_container(MyStruct_addInt)
add_container(MyStruct_intGetter)

endif (BUILD_SIMPLE_EXAMPLE)
