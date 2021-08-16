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


add_container(TurtleSim)
add_container(TurtleSim_spawn)
add_container(TurtleSim_getPose)
add_container(TurtleSim_update)
add_container(TurtleSim_kill)
add_container(TurtleSim_setVelocity)
add_container(TurtleSim_odomTransform)

set(TURTLESIM_MODULES ${OPERATIONS} ${CONTAINERS} ${ECS} PARENT_SCOPE)

INSTALL(TARGETS ${CONTAINERS} ${OPERATIONS} DESTINATION share/juiz/example/turtlesim)
INSTALL(FILES turtlesim.nkshelf DESTINATION share/juiz/example/turtlesim)

