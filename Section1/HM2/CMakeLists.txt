cmake_minimum_required(VERSION 3.10)


project(H2)

set(sum_array)

include_directories(include)


set(CMAKE_C_STANDARD 11)


add_executable(sum_array
  src/main.c
  src/functions.c)
