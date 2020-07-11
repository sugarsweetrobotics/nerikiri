cmake_minimum_required(VERSION 3.5)

# add_compile_options(-std=gnu++17)


project(nerikiri LANGUAGES CXX C VERSION 0.0.1)

enable_testing()
option(BUILD_OPENCV_EXAMPLE "Build OpenCV Example" ON)
option(BUILD_JOY_EXAMPLE "Build Joy Controller Example" ON)
option(BUILD_TURTLESIM_EXAMPLE "Build Turtle Simulator Controller Example" ON)

set(CMAKE_USE_RELATIVE_PATHS True)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(thirdparty)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(example)
add_subdirectory(apps)
add_subdirectory(test)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${nk_public_headers}")

INSTALL(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib 
        PUBLIC_HEADER DESTINATION include/nerikiri/)


set(CPACK_GENERATOR WIX)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "SUGAR SWEET ROBOTICS")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "nerikiri - A robot middleware")
set(CPACK_PACKAGE_VERSION "0.0.1")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "nerikiri")
SET(CPACK_NSIS_MODIFY_PATH ON)

INCLUDE(CPack)