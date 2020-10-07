

if (BUILD_EXAMPLE)

option(BUILD_OPENCV_EXAMPLE "Build OpenCV Example" ON)
option(BUILD_JOY_EXAMPLE "Build Joy Controller Example" ON)
option(BUILD_TURTLESIM_EXAMPLE "Build Turtle Simulator Controller Example" ON)


set(example01_src example01.cpp)
set(example02_src example02.cpp)
set(example03_src example03.cpp)
set(example04_src example04.cpp)
set(example05_src example05.cpp)
set(example06_src example06.cpp)
set(example07_src example07.cpp)
set(example08_src example08.cpp)
set(example09_src example09.cpp)
set(example10_src example10.cpp)


find_package(Threads REQUIRED)

set(EXAMPLE_LIBS nerikiri)
set(EXTRA_MODULES one)

set(EXAMPLES)


set(OPERATIONS)
function(add_operation NAME)
  add_library(${NAME} SHARED operations/${NAME}.cpp)
  add_dependencies(${NAME} nerikiri)

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
  add_dependencies(${NAME} nerikiri)

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
  add_dependencies(${NAME} nerikiri)
  add_dependencies(${NAME} ${OPERATIONS})
endfunction(add_example)

if (BUILD_SIMPLE_EXAMPLE)
add_operation(one)
add_operation(increment)
add_operation(decrement)
add_operation(add)
add_operation(zero)
add_container(MyStruct MyStruct)
add_container(MyStruct MyStruct_addInt)
add_container(MyStruct MyStruct_intGetter)
endif (BUILD_SIMPLE_EXAMPLE)

if (BUILD_TURTLESIM_EXAMPLE)
add_subdirectory(turtlesim)
list(APPEND EXAMPLES turtlesim.nkproj)
endif (BUILD_TURTLESIM_EXAMPLE)

if (BUILD_JOY_EXAMPLE)
add_subdirectory(joy)
list(APPEND EXAMPLES keyjoy.nkproj)
endif (BUILD_JOY_EXAMPLE)

if (BUILD_OPENCV_EXAMPLE)
add_subdirectory(opencv)
list(APPEND EXAMPLES opencv.nkproj)
endif (BUILD_OPENCV_EXAMPLE)

set(EXAMPLE_MODULES ${OPERATIONS} ${CONTAINERS} ${ECS} PARENT_SCOPE)

INSTALL(FILES ${EXAMPLES} DESTINATION share/nerikiri/example/)


endif(BUILD_EXAMPLE)
