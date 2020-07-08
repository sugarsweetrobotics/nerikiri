include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libaqua2/include)

set(OPERATIONS)

set(EXT_LIBS)

if (APPLE)
find_library( FOUNDATION_LIBRARY Foundation )
find_library( IOKIT_LIBRARY IOKit )
set(EXT_LIBS ${IOKIT_LIBRARY} ${FOUNDATION_LIBRARY})
endif (APPLE)


function(add_operation NAME)
  add_library(${NAME} SHARED ${NAME}.cpp)
  add_dependencies(${NAME} nerikiri)
  target_link_libraries(${NAME} ${EXT_LIBS})
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
  add_dependencies(${NAME} nerikiri)
  target_link_libraries(${NAME} ${EXT_LIBS})

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

add_operation(joy2turtlevel)

add_container(JoyController)
add_container(JoyController_initialize)
add_container(JoyController_update)
add_container(JoyController_get)


add_container(KeyboardJoy)
add_container(KeyboardJoy_initialize)
add_container(KeyboardJoy_get)
add_container(KeyboardJoy_update)

set(JOY_MODULES ${OPERATIONS} ${CONTAINERS} PARENT_SCOPE)


INSTALL(TARGETS ${CONTAINERS} ${OPERATIONS} DESTINATION share/nerikiri/example/joy)
INSTALL(FILES joy.nkshelf DESTINATION share/nerikiri/example/joy)

