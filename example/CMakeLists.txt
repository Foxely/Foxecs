cmake_minimum_required(VERSION 3.17)

add_executable(example main.cpp)
target_include_directories(example PRIVATE "include")
target_link_libraries(example fox_ecs)
