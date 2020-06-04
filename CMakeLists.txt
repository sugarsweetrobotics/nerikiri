cmake_minimum_required(VERSION 3.5)

# add_compile_options(-std=gnu++17)


project(nerikiri LANGUAGES CXX C VERSION 0.0.1)

enable_testing()
option(BUILD_OPENCV_EXAMPLE "Build OpenCV Example" ON)


set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(thirdparty)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(example)
add_subdirectory(apps)

#add_subdirectory(test)
