cmake_minimum_required(VERSION 3.10)

project(data_processor)

# Include the current directory for header files 
include_directories(include)

set(CMAKE_C_STANDARD 11)

add_executable(mq_exam_1
   src/main.c)

target_link_libraries(mq_exam_1
   m)