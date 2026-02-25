cmake_minimum_required(VERSION 3.16)
project(simple_sdl)

# Output Directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL) # SDL in vendored/

add_executable(simple_sdl entry.c)
target_link_libraries(simple_sdl PRIVATE SDL3::SDL3)