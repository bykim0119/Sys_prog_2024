cmake_minimum_required(VERSION 3.5.0)

project(leveldb_bench VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug) #디버깅 플래그 설정

set(LEVELDB_DIR "${CMAKE_SOURCE_DIR}/leveldb")

# 경로 지정 (leveldb 위치)
include_directories(${LEVELDB_DIR}/include)
link_directories(${LEVELDB_DIR}/build)

add_executable(leveldb_bench main.cpp)

target_link_libraries(leveldb_bench leveldb)
