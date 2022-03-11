message("---------------------- ComponentName project --------------------------")

cmake_minimum_required(VERSION 3.10)

project(ComponentName LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CONTAINERS)
set(CONTAINER_OPERATIONS)
set(OPERATIONS)

find_package(juiz)

message("- juiz_ROOT_DIR   : " ${juiz_ROOT_DIR})
message("- juiz_INCLUDE_DIR: " ${juiz_INCLUDE_DIR})
message("- juiz_LIB_DIR    : " ${juiz_LIB_DIR})
message("- juiz_LIBRARIES  : " ${juiz_LIBRARIES})

set(JUIZ_LIBS ${juiz_LIBRARIES})
set(EXT_LIBS)

include(juiz_component.cmake)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${juiz_INCLUDE_DIR})
link_directories(${juiz_LIB_DIR})

# Output basic information of this project.
message("- CMakeLists.txt    : " ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)
message("- JUIZ_LIBS         : " ${JUIZ_LIBS})
message("- EXT_LIBS          : " ${EXT_LIBS})

# ----- IMPLEMENTATION AREA ------

# Add projects to target
add_containers(CONTAINERS)
add_container_operations(CONTAINER_OPERATIONS)
add_operations(OPERATIONS)

# Define modules for install
set(COMPONENTNAME_MODULES ${OPERATIONS} ${CONTAINERS} ${ECS} PARENT_SCOPE)

# Installing files for targets
INSTALL(TARGETS ${CONTAINERS} ${CONTAINER_OPERATIONS} ${OPERATIONS} DESTINATION ${juiz_ROOT_DIR}/share/juiz/components/ComponentName)
INSTALL(FILES ComponentName.jshelf DESTINATION ${juiz_ROOT_DIR}/share/juiz/components/ComponentName)



message("-----------------------------------------------------------------------")