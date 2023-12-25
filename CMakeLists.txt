cmake_minimum_required(VERSION 3.5.7)
project(Loppi VERSION 0.1.0 LANGUAGES C CXX)

set (CMAKE_CXX_STANDARD 20)
add_subdirectory(classes)
add_executable(main main.cpp)

target_link_libraries(main PUBLIC cls)
