cmake_minimum_required(VERSION 3.10)


project(H2)

set(ticket_office)

include_directories(include)


set(CMAKE_C_STANDARD 11)


add_executable(ticket_office
  src/main.c
  src/functions.c)

target_link_libraries(ticket_office
  m)

