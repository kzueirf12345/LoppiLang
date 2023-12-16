cmake_minimum_required(VERSION 3.0.0)
project(Loppi VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(classes)
add_executable(main main.cpp)

target_link_libraries(main PUBLIC cls)
