cmake_minimum_required(VERSION 3.10)


project(L2)

set(theater)

include_directories(include)


set(CMAKE_C_STANDARD 11)


add_executable(theater
  src/main.c
  src/functions.c)