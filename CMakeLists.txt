cmake_minimum_required(VERSION 3.5)
add_compile_options(-std=gnu++17)

# enable_testing()

project(paio LANGUAGES CXX VERSION 0.0.1)

add_subdirectory(src)
add_subdirectory(example)

# add_subdirectory(test)
