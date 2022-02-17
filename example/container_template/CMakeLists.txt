message("---------------------- ComponentName project --------------------------")


project(ComponentName LANGUAGES CXX C VERSION 0.0)

set(CONTAINERS)
set(CONTAINER_OPERATIONS)
set(OPERATIONS)

set(JUIZ_LIBS juiz)
set(EXT_LIBS)

include(juiz_component.cmake)

include_directories(${CMAKE_SOURCE_DIR}/include)

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
set(COMPONENTNAME_MODULES ${EXPORTING_OPERATIONS} ${EXPORTING_CONTAINERS} ${ECS} PARENT_SCOPE)

# Installing files for targets
INSTALL(TARGETS ${EXPORTING_CONTAINERS} ${EXPORTING_OPERATIONS} DESTINATION share/juiz/example/componentname)
INSTALL(FILES componentname.jshelf DESTINATION share/juiz/example/componentname)

message("-----------------------------------------------------------------------")