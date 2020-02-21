cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_compile_options(-std=gnu++17)


enable_testing()

project(nerikiri LANGUAGES CXX VERSION 0.0.1)

add_subdirectory(thirdparty)

add_subdirectory(src)
add_subdirectory(example)

add_subdirectory(test)
