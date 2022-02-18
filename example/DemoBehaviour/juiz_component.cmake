# ---------------- DO NOT EDIT FROM HERE ------------------
# Author do not recommend to edit this file contents below
# 
#
# ---------------------------------------------------------

set(EXPORTING_OPERATIONS)
set(EXPORTING_CONTAINERS)

# --- FUNCTION DEFINITION

# Add Operation to target
function(add_operation NAME)
message("- Operation         : " ${NAME})
  add_library(${NAME} SHARED ${NAME}.cpp)
  add_dependencies(${NAME} ${JUIZ_LIBS})
  target_link_libraries(${NAME} ${JUIZ_LIBS} ${EXT_LIBS})
  set_target_properties(${NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  )
  set_target_properties(${NAME} PROPERTIES PREFIX "")
  set(EXPORTING_OPERATIONS ${NAME} ${EXPORTING_OPERATIONS})
  set(EXPORTING_OPERATIONS ${EXPORTING_OPERATIONS} PARENT_SCOPE)
endfunction(add_operation)

# Add Container to target.
# This function is used for ContainerOperation as well
function(add_container_impl NAME)
  add_library(${NAME} SHARED ${NAME}.cpp)
  add_dependencies(${NAME} ${JUIZ_LIBS})
  target_link_libraries(${NAME} ${JUIZ_LIBS} ${EXT_LIBS})
  set_target_properties(${NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  )
  set_target_properties(${NAME} PROPERTIES PREFIX "")
  set(EXPORTING_CONTAINERS ${NAME} ${EXPORTING_CONTAINERS})
  set(EXPORTING_CONTAINERS ${EXPORTING_CONTAINERS} PARENT_SCOPE)
endfunction(add_container_impl)

# Add Container to target
function(add_container NAME)
 message("- Container         : " ${NAME})
  add_container_impl(${NAME})
endfunction(add_container)

# Add ContainerOperation to target
function(add_container_operation NAME)
 message("- ContainerOperation: " ${NAME})
  add_container_impl(${NAME})
endfunction(add_container_operation)

# Add Containers. This function handles lists of containers
function(add_containers NAMES)
  foreach(NAME IN LISTS ${NAMES})
    add_container(${NAME})
  endforeach()
endfunction(add_containers)

# Add ContainerOperations. This function handles lists of container operations
function(add_container_operations NAMES)
  foreach(NAME IN LISTS ${NAMES})
    add_container_operation(${NAME})
  endforeach()
endfunction(add_container_operations)

# Add Operations. This function handles lists of operations
function(add_operations NAMES)
  foreach(NAME IN LISTS ${NAMES})
    add_operation(${NAME})
  endforeach()
endfunction(add_operations)