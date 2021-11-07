cmake_minimum_required(VERSION 3.17)

add_executable(basic main.cpp)
target_include_directories(basic PRIVATE "include")
target_link_libraries(basic fox_ecs)
