cmake_minimum_required(VERSION 3.10)

project(demo)

add_definitions("-Wall -g")
add_definitions(-std=c++17)
link_libraries(pthread)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

include_directories("${PROJECT_SOURCE_DIR}/../CatLog")
add_subdirectory(${PROJECT_SOURCE_DIR}/../CatLog CatLog.out)
set (EXTRA_LIBS0 ${EXTRA_LIBS0} CatLog_Sington)

aux_source_directory(. DIR_LIB_SRCS)

add_executable(demo ${DIR_LIB_SRCS})
target_link_libraries(demo ${EXTRA_LIBS0})
