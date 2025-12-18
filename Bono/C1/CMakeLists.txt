cmake_minimum_required(VERSION 3.10)

project(time_plotter)

# Include the current directory for header files 
include_directories(include)

set(CMAKE_C_STANDARD 11)

add_executable(time_plotter
   src/main.c)

add_executable(alarm_driver
   src/alarm_driver.c)

target_link_libraries(time_plotter
   m)