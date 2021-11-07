cmake_minimum_required(VERSION 3.17)

add_executable(sfml_example main.cpp)
target_include_directories(sfml_example PRIVATE "include")
target_link_libraries(sfml_example fox_ecs)
