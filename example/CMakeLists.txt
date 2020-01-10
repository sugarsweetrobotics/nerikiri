set(example01_src example01.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)
set(EXAMPLE_LIBS nerikiri)

find_package(Threads REQUIRED)

function(add_example NAME)
  add_executable(${NAME} ${${NAME}_src})
  target_link_libraries(${NAME} ${EXAMPLE_LIBS} Threads::Threads)
endfunction(add_example)


add_example(example01)
