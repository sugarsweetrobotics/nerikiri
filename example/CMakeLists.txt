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

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/libaqua2/include)

find_package(Threads REQUIRED)

# add_library(one SHARED one.cpp)
# target_link_libraries(one nerikiri Threads::Threads)

set(EXAMPLE_LIBS nerikiri)
set(EXTRA_MODULES one)

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

  add_dependencies(${NAME} nerikiri)
  add_dependencies(${NAME} ${OPERATIONS})
endfunction(add_example)


add_operation(one)
add_operation(increment)
add_operation(decrement)
add_operation(add)
add_operation(zero)
add_operation(joy2turtlevel)

add_container(MyStruct MyStruct)


add_container(MyStruct MyStruct_addInt)

add_container(MyStruct MyStruct_intGetter)

add_container(TurtleSim TurtleSim)
add_container(TurtleSim TurtleSim_spawn)
add_container(TurtleSim TurtleSim_kill)
add_container(TurtleSim TurtleSim_update)
add_container(TurtleSim TurtleSim_getPose)
add_container(TurtleSim TurtleSim_setVelocity)


add_container(JoyController JoyController)
add_container(JoyController JoyController_initialize)
add_container(JoyController JoyController_update)
add_container(JoyController JoyController_get)


if (APPLE)
find_library( FOUNDATION_LIBRARY Foundation )
find_library( IOKIT_LIBRARY IOKit )
target_link_libraries(JoyController_initialize ${IOKIT_LIBRARY} ${FOUNDATION_LIBRARY})
endif (APPLE)

add_example(turtle_sim)
#add_example(example01)
#add_example(example02)
#add_example(example03)
#add_example(example04)
#add_example(example05)
#add_example(example06)
#add_example(example07)
#add_example(example08)
#add_example(example09)
#add_example(example10)

if (FALSE)
endif()

set(EXAMPLE_MODULES ${OPERATIONS} ${CONTAINERS} ${ECS} PARENT_SCOPE)