set(nk_base_srcs process.cpp operation.cpp state.cpp statemap.cpp)
set(nk_http_srcs http/restbroker.cpp)
set(nk_srcs ${nk_base_srcs} ${nk_http_srcs})

include_directories(${CMAKE_SOURCE_DIR}/include)

set(NK_BASE_LIBS)

add_library(nerikiri ${nk_srcs})
target_link_libraries(nerikiri ${NK_BASE_LIBS})


