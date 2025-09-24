cmake_minimum_required(VERSION 3.10)


project(L1)

set(parent_program)

include_directories(include)


set(CMAKE_C_STANDARD 11)


add_executable(parent_program
  src/main.c
)


