cmake_minimum_required(VERSION 3.10)

project(test)

add_definitions("-Wall -g")
link_libraries(pthread)

aux_source_directory(. DIR_LIB_SRCS)

add_executable(test ${DIR_LIB_SRCS})
