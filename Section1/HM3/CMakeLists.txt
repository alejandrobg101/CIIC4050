cmake_minimum_required(VERSION 3.10)


project(H2)

set(hurricane_app)

include_directories(include)


set(CMAKE_C_STANDARD 11)


add_executable(hurricane_app
  src/main.c
  src/functions.c)

target_link_libraries(hurricane_app
  m)