include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/libaqua2/include)

set(OPERATIONS)

find_package(OpenCV REQUIRED)
function(add_operation NAME)
  add_library(${NAME} SHARED ${NAME}.cpp)
  add_dependencies(${NAME} nerikiri)
  target_link_libraries(${NAME} ${OpenCV_LIBS})
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
  target_link_libraries(${NAME} ${OpenCV_LIBS})

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


find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_container(OpenCVCamera)
add_container(OpenCVCamera_initialize)
add_container(OpenCVCamera_capture)
add_container(OpenCVCascadeClassifier)
add_container(OpenCVCascadeClassifier_initialize)
add_container(OpenCVCascadeClassifier_detectMultiScale)
add_operation(cv_drawRects)
add_operation(cv_flip)

set(OPENCV_MODULES ${OPERATIONS} ${CONTAINERS} ${ECS} PARENT_SCOPE)

INSTALL(TARGETS ${CONTAINERS} ${OPERATIONS} DESTINATION share/nerikiri/example/opencv)
INSTALL(FILES opencv.nkshelf DESTINATION share/nerikiri/example/opencv)

