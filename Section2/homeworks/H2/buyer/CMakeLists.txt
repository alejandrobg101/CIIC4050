cmake_minimum_required(VERSION 3.10)


project(H2)

set(buyer)

include_directories(include)


set(CMAKE_C_STANDARD 11)


add_executable(buyer
  src/main.c
  src/functions.c)

target_link_libraries(buyer
  m)

