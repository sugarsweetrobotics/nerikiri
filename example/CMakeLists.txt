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
add_operation(joy2turtlevel)
add_container(TurtleSim TurtleSim)
add_container(TurtleSim TurtleSim_spawn)
add_container(TurtleSim TurtleSim_kill)
add_container(TurtleSim TurtleSim_update)
add_container(TurtleSim TurtleSim_getPose)
add_container(TurtleSim TurtleSim_setVelocity)
endif (BUILD_TURTLESIM_EXAMPLE)

if (BUILD_JOYCONTROLLER_EXAMPLE)
add_container(JoyController JoyController)
add_container(JoyController JoyController_initialize)
add_container(JoyController JoyController_update)
add_container(JoyController JoyController_get)

if (APPLE)
find_library( FOUNDATION_LIBRARY Foundation )
find_library( IOKIT_LIBRARY IOKit )
target_link_libraries(JoyController_initialize ${IOKIT_LIBRARY} ${FOUNDATION_LIBRARY})
endif (APPLE)

endif (BUILD_JOYCONTROLLER_EXAMPLE)

if (BUILD_OPENCV_EXAMPLE)
add_subdirectory(opencv)
list(APPEND EXAMPLES opencv.nkproj)
endif (BUILD_OPENCV_EXAMPLE)
#find_package(OpenCV REQUIRED)
#include_directories(${OpenCV_INCLUDE_DIRS})
#add_container(OpenCVCamera  OpenCVCamera)
#add_container(OpenCVCamera  OpenCVCamera_initialize)
#add_container(OpenCVCamera  OpenCVCamera_capture)#
#
#add_container(OpenCVCascadeClassifier  OpenCVCascadeClassifier)
#add_container(OpenCVCascadeClassifier  OpenCVCascadeClassifier_initialize)
##add_container(OpenCVCascadeClassifier  OpenCVCascadeClassifier_detectMultiScale)
#add_operation(cv_drawRects)#
#
#add_operation(cv_flip)
#target_link_libraries(OpenCVCamera ${OpenCV_LIBS})
#target_link_libraries(OpenCVCamera_initialize ${OpenCV_LIBS})
#target_link_libraries(OpenCVCamera_capture ${OpenCV_LIBS})
#target_link_libraries(cv_flip ${OpenCV_LIBS})
#target_link_libraries(cv_drawRects ${OpenCV_LIBS})
#target_link_libraries(OpenCVCascadeClassifier ${OpenCV_LIBS})
#target_link_libraries(OpenCVCascadeClassifier_initialize ${OpenCV_LIBS})
#target_link_libraries(OpenCVCascadeClassifier_detectMultiScale ${OpenCV_LIBS})
#endif (BUILD_OPENCV_EXAMPLE)



# add_example(turtle_sim)
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


INSTALL(FILES ${EXAMPLES} DESTINATION share/nerikiri/example/)