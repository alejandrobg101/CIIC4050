cmake_minimum_required(VERSION 3.10)


project(H1)

set(tell_joke)

include_directories(include)


set(CMAKE_C_STANDARD 11)


add_executable(tell_joke
  src/main.c
  src/functions.c)
