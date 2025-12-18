cmake_minimum_required(VERSION 3.10)

project(vruntime_logger)

include_directories(include)

set(CMAKE_CXX_FLAGS_DEBUG "-pg -Og -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(${CMAKE_PROJECT_NAME} 
    src/main.c
    )

target_link_libraries(${CMAKE_PROJECT_NAME}
    m)