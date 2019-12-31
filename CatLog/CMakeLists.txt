cmake_minimum_required(VERSION 3.10)

project(CatLog)

add_definitions("-Wall -g")
add_definitions(-std=c++11)
link_libraries(pthread)

aux_source_directory(. DIR_LIB_SRCS)

add_library(CatLog SHARED ${DIR_LIB_SRCS})
