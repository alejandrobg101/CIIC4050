cmake_minimum_required(VERSION 3.10)

project(L1_Solved)

# Include the current directory for header files 
include_directories(include)

set(CMAKE_C_STANDARD 11)

add_executable(L1_Solved
   src/main.c

target_link_libraries(L1_Solved
   m)