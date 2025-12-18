cmake_minimum_required(VERSION 3.10)

project(LAB4_L1)

# Include the current directory for header files 
include_directories(include)

set(CMAKE_C_STANDARD 11)

add_executable(LAB4_L1
   src/main.c
   src/functions.c)

target_link_libraries(LAB4_L1
   m)