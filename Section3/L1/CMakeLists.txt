cmake_minimum_required(VERSION 3.10)


project(L1)

set(horse_race)

set(CMAKE_C_STANDARD 11)


add_executable(horse_race
  src/main.c
  )

target_link_libraries(horse_race
  m)

