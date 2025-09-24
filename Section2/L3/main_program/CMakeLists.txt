cmake_minimum_required(VERSION 3.10)

project(main_program)

# Include the current directory for header files 
include_directories(include)

set(CMAKE_C_STANDARD 11)

add_executable(main
   src/main.c)

target_link_libraries(main
   m)