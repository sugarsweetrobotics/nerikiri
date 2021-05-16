
set(OPERATIONS)

function(add_operation NAME)
  add_library(${NAME} SHARED operations/${NAME}.cpp)
  add_dependencies(${NAME} juiz)

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
function(add_container DIRNAME NAME)
  add_library(${NAME} SHARED containers/${DIRNAME}/${NAME}.cpp)
  add_dependencies(${NAME} juiz)

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

function(add_example NAME)
  add_executable(${NAME} ${NAME}.cpp)
  target_link_libraries(${NAME} ${EXAMPLE_LIBS} Threads::Threads)

  set_target_properties(${NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  )

  set_target_properties(${NAME} PROPERTIES PREFIX "")
  add_dependencies(${NAME} juiz)
  add_dependencies(${NAME} ${OPERATIONS})
endfunction(add_example)

find_package(Threads REQUIRED)


option(BUILD_JUIZ_EXAMPLE_SIMPLE "Build Simple Operation Examples. This is used in tests" ON)
option(BUILD_JUIZ_EXAMPLE_OPENCV "Build OpenCV Example" OFF)
option(BUILD_JUIZ_EXAMPLE_JOY "Build Joy Controller Example" OFF)
option(BUILD_JUIZ_EXAMPLE_TURTLESIM "Build Turtle Simulator Controller Example" OFF)


set(EXAMPLE_LIBS juiz)
set(EXTRA_MODULES one)

set(EXAMPLES brokers.nkshelf ecs.nkshelf )

if (BUILD_JUIZ_EXAMPLE_SIMPLE)
  add_subdirectory(simple_example)
endif (BUILD_JUIZ_EXAMPLE_SIMPLE) 


if (BUILD_JUIZ_EXAMPLE_TURTLESIM)
  add_subdirectory(turtlesim)
  list(APPEND EXAMPLES turtlesim.nkproj)
endif (BUILD_JUIZ_EXAMPLE_TURTLESIM)

if (BUILD_JUIZ_EXAMPLE_JOY)
  add_subdirectory(joy)
  list(APPEND EXAMPLES keyjoy.nkproj)
endif (BUILD_JUIZ_EXAMPLE_JOY)

if (BUILD_JUIZ_EXAMPLE_OPENCV)
  add_subdirectory(opencv)
  list(APPEND EXAMPLES opencv.nkproj)
endif (BUILD_JUIZ_EXAMPLE_OPENCV)

set(EXAMPLE_MODULES ${OPERATIONS} ${CONTAINERS} ${ECS} PARENT_SCOPE)

INSTALL(FILES ${EXAMPLES} DESTINATION share/juiz/example/)


