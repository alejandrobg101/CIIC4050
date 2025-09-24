cmake_minimum_required(VERSION 3.10)

project(worker)

# Include the current directory for header files 
include_directories(include)

set(CMAKE_C_STANDARD 99)
add_compile_definitions(_POSIX_C_SOURCE=200809L)

add_executable(worker
   src/main.c)

target_link_libraries(worker
   m)