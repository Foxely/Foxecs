cmake_minimum_required(VERSION 3.17)

file(GLOB_RECURSE SRCS "src/*.cpp")

add_library(fox_ecs STATIC ${SRCS})
target_include_directories(fox_ecs PUBLIC "include")
