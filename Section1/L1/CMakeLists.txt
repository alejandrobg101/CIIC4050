cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -s")

project(CalculateMedia)
include_directories(include)

add_executable(calculate_media
  src/main.c
  src/functions.c)