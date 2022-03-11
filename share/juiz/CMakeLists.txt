set(TEMPLATE_FILES
  component_template/CMakeLists.txt
  component_template/juiz_component.cmake
  # component_template/ComponentName.jshelf
  component_template/ContainerName.h
  component_template/ContainerName.cpp
  component_template/ContainerName_containerOperationName.cpp
  component_template/operationName.cpp
)

set(CORE_PROJECTS
  brokers.jshelf
  ecs.jshelf
)

set(INCLUDE_INSTALL_DIR)

install(FILES ${CORE_PROJECTS} DESTINATION share/juiz)
install(FILES ${TEMPLATE_FILES} DESTINATION share/juiz/component_template)

message(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(JUIZ_INSTALL_PREFIX "/opt/juiz/share/juiz")



include(CMakePackageConfigHelpers)
configure_package_config_file(component_template/ComponentName.jshelf.in
  ${CMAKE_CURRENT_BINARY_DIR}/component_template/ComponentName.jshelf
  INSTALL_DESTINATION share/juiz/component_template
  PATH_VARS JUIZ_INSTALL_PREFIX)


install(FILES ${CMAKE_CURRENT_BINARY_DIR}/component_template/ComponentName.jshelf
  DESTINATION share/juiz/component_template)
