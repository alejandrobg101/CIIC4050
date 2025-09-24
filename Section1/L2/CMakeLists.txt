cmake_minimum_required(VERSION 3.10)


project(L2)

set(calc_geo)

include_directories(include)


set(CMAKE_C_STANDARD 11)


add_executable(${calc_geo}
  src/main.c
  src/functions.c)

target_link_libraries(${calc_geo}
  m)

