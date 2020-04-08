set(example01_src example01.cpp)
set(example02_src example02.cpp)
set(example03_src example03.cpp)
set(example04_src example04.cpp)
set(example05_src example05.cpp)
set(example06_src example06.cpp)
set(example07_src example07.cpp)
set(example08_src example08.cpp)
set(example09_src example09.cpp)
set(example10_src example10.cpp)



include_directories(${CMAKE_SOURCE_DIR}/include)
set(EXAMPLE_LIBS nerikiri)

find_package(Threads REQUIRED)

function(add_example NAME)
  add_executable(${NAME} ${${NAME}_src})
  target_link_libraries(${NAME} ${EXAMPLE_LIBS} Threads::Threads)
endfunction(add_example)


add_example(example01)
add_example(example02)
add_example(example03)
add_example(example04)
add_example(example05)
add_example(example06)
add_example(example07)
add_example(example08)
add_example(example09)
add_example(example10)
