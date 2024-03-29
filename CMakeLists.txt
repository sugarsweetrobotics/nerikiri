cmake_minimum_required(VERSION 3.5)

# add_compile_options(-std=gnu++17)
set(JUIZ_VERSION 0.0.1)
set(JUIZ_VERSION_MAJOR 0)
set(JUIZ_VERSION_MINOR 0)
set(JUIZ_VERSION_SUBMINOR 1)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#  set(CMAKE_C_COMPILER "gcc")
# set(CMAKE_C_COMPILER "clang")
#  set(CMAKE_CXX_COMPILER "g++")
# set(CMAKE_CXX_COMPILER "clang++")
endif()

# set(CMAKE_C_COMPILER "clang")
#  set(CMAKE_CXX_COMPILER "g++")
# set(CMAKE_CXX_COMPILER "clang++")

project(juiz LANGUAGES CXX C VERSION ${JUIZ_VERSION})

enable_testing()

option(BUILD_JUIZ_EXAMPLE "Build Example" OFF)
option(BUILD_JUIZ_TEST "Build JUIZ's Test" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# set(CMAKE_USE_RELATIVE_PATHS True)
add_subdirectory(thirdparty)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(share/juiz)

if (BUILD_JUIZ_EXAMPLE)
message("--- Building Examples")
add_subdirectory(example)
endif (BUILD_JUIZ_EXAMPLE)

add_subdirectory(apps)
add_subdirectory(test)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${juiz_public_headers}")
message(${juiz_public_util_headers})
set(INCLUDE_INSTALL_DIR include/ )
set(RUNTIME_INSTALL_DIR bin/ )
set(LIB_INSTALL_DIR lib/ )
set(SYSCONFIG_INSTALL_DIR etc/${PROJECT_NAME}/ )

INSTALL(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${RUNTIME_INSTALL_DIR}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        PUBLIC_HEADER DESTINATION include/${PROJECT_NAME})

INSTALL(FILES ${juiz_public_util_headers} DESTINATION include/${PROJECT_NAME}/utils )

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/${PROJECT_NAME}/cmake
  PATH_VARS INCLUDE_INSTALL_DIR SYSCONFIG_INSTALL_DIR LIB_INSTALL_DIR)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${JUIZ_VERSION}
        COMPATIBILITY SameMajorVersion )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${LIB_INSTALL_DIR}/${PROJECT_NAME}/cmake )

set(CPACK_GENERATOR WIX)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "SUGAR SWEET ROBOTICS")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "JUIZ - A robot middleware")
set(CPACK_PACKAGE_VERSION ${JUIZ_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${JUIZ_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${JUIZ_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${JUIZ_VERSION_SUBMINOR})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "juiz")
SET(CPACK_NSIS_MODIFY_PATH ON)

INCLUDE(CPack)
