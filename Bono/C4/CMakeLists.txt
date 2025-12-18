cmake_minimum_required(VERSION 3.10)
project(c4_shared_mem)

set(CMAKE_C_STANDARD 11)

add_executable(encrypt_writer
    src/encrypt_writer.c)

add_executable(decrypt_reader
    src/decrypt_reader.c)

# Link librt for POSIX shm if needed on your system
target_link_libraries(encrypt_writer rt)
target_link_libraries(decrypt_reader rt)