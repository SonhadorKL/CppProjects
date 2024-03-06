cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_STANDARD 20)

project(ListAndMap)

add_library(ListAndMap INTERFACE)

target_include_directories(ListAndMap INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)