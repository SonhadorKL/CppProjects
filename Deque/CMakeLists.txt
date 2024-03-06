cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_STANDARD 20)

project(Deque)

add_library(Deque INTERFACE)

target_include_directories(Deque INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})