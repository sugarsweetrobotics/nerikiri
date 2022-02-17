include_directories(${CMAKE_SOURCE_DIR}/include)

set(CONTAINERS ContainerName)
set(EXPORTING_EXPORTING_OPERATIONS)
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

  set(EXPORTING_EXPORTING_OPERATIONS ${NAME} ${EXPORTING_EXPORTING_OPERATIONS})
  set(EXPORTING_EXPORTING_OPERATIONS ${EXPORTING_EXPORTING_OPERATIONS} PARENT_SCOPE)
endfunction(add_operation)

set(EXPORTING_CONTAINERS)
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
  set(EXPORTING_CONTAINERS ${NAME} ${EXPORTING_CONTAINERS})
  set(EXPORTING_CONTAINERS ${EXPORTING_CONTAINERS} PARENT_SCOPE)
endfunction(add_container)


add_container(TurtleSim)
add_container(TurtleSim_spawn)
add_container(TurtleSim_getPose)
add_container(TurtleSim_update)
add_container(TurtleSim_kill)
add_container(TurtleSim_setVelocity)
add_container(TurtleSim_odomTransform)

set(TURTLESIM_MODULES ${EXPORTING_EXPORTING_OPERATIONS} ${EXPORTING_CONTAINERS} ${ECS} PARENT_SCOPE)

INSTALL(TARGETS ${EXPORTING_CONTAINERS} ${EXPORTING_EXPORTING_OPERATIONS} DESTINATION share/juiz/example/turtlesim)
INSTALL(FILES turtlesim.nkshelf DESTINATION share/juiz/example/turtlesim)

