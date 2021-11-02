cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
project(FoxEcs VERSION 1.0.0 LANGUAGES CXX)

set(FOXECS_EXAMPLE 1)

add_subdirectory(project)

if(FOXECS_EXAMPLE)
    add_subdirectory(example)
endif()
