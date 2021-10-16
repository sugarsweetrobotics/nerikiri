

add_subdirectory(Catch2)
add_subdirectory(spdlog)

option(YAML_BUILD_SHARED_LIBS OFF)
include_directories(yaml-cpp/include)
add_subdirectory(yaml-cpp)
